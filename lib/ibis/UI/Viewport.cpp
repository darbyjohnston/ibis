// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "Viewport.h"

#include <ibis/Models/Document.h>
#include <ibis/Models/TimeModel.h>

#include <ibis/Render/Graph.h>
#include <ibis/Render/INode.h>

#include <ftk/GL/GL.h>
#include <ftk/GL/OffscreenBuffer.h>
#include <ftk/GL/Util.h>
#include <ftk/Core/RenderUtil.h>

namespace ibis
{
    namespace ui
    {
        struct Viewport::Private
        {
            std::shared_ptr<models::Document> document;
            std::shared_ptr<render::INode> node;
            OTIO_NS::RationalTime currentTime;

            int sizeHint = 0;

            bool doRender = true;
            std::shared_ptr<ftk::gl::OffscreenBuffer> buffer;

            std::shared_ptr<ftk::ListObserver<std::shared_ptr<render::INode> > > nodesObserver;
            std::shared_ptr<ftk::Observer<bool> > changedObserver;
            std::shared_ptr<ftk::Observer<OTIO_NS::RationalTime> > currentTimeObserver;
            std::shared_ptr<ftk::Observer<std::shared_ptr<render::INode> > > viewNodeObserver;
        };

        void Viewport::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::ui::Viewport", parent);
            FTK_P();

            p.document = document;

            p.nodesObserver = ftk::ListObserver<std::shared_ptr<render::INode> >::create(
                document->getGraph()->observeNodes(),
                [this](const std::vector<std::shared_ptr<render::INode> >& nodes)
                {
                    FTK_P();
                    const auto i = std::find(nodes.begin(), nodes.end(), p.node);
                    if (i == nodes.end())
                    {
                        p.node.reset();
                        p.doRender = true;
                        setDrawUpdate();
                    }
                });

            p.changedObserver = ftk::Observer<bool>::create(
                document->getGraph()->observe(),
                [this](bool value)
                {
                    FTK_P();
                    if (value)
                    {
                        p.doRender = true;
                        setDrawUpdate();
                    }
                });

            p.currentTimeObserver = ftk::Observer<OTIO_NS::RationalTime>::create(
                document->getTimeModel()->observeCurrentTime(),
                [this](const OTIO_NS::RationalTime& value)
                {
                    FTK_P();
                    p.currentTime = value;
                    p.doRender = true;
                    setDrawUpdate();
                });

            p.viewNodeObserver = ftk::Observer<std::shared_ptr<render::INode> >::create(
                document->observeViewNode(),
                [this](const std::shared_ptr<render::INode>& node)
                {
                    FTK_P();
                    p.node = node;
                    p.doRender = true;
                    setDrawUpdate();
                });
        }

        Viewport::Viewport() :
            _p(new Private)
        {}

        Viewport::~Viewport()
        {}

        std::shared_ptr<Viewport> Viewport::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<Viewport> out(new Viewport);
            out->_init(context, document, parent);
            return out;
        }

        ftk::Size2I Viewport::getSizeHint() const
        {
            FTK_P();
            return ftk::Size2I(p.sizeHint, p.sizeHint);
        }

        void Viewport::setGeometry(const ftk::Box2I& value)
        {
            const bool changed = value != getGeometry();
            IWidget::setGeometry(value);
            FTK_P();
            if (changed)
            {
                p.doRender = true;
                setDrawUpdate();
            }
        }

        void Viewport::sizeHintEvent(const ftk::SizeHintEvent& event)
        {
            IWidget::sizeHintEvent(event);
            FTK_P();
            p.sizeHint = event.style->getSizeRole(ftk::SizeRole::ScrollArea, event.displayScale);
        }

        void Viewport::drawEvent(const ftk::Box2I& drawRect, const ftk::DrawEvent& event)
        {
            FTK_P();
            const ftk::Box2I& g = getGeometry();
            if (p.doRender)
            {
                p.doRender = false;

                const ftk::Size2I size = g.size();
                ftk::gl::OffscreenBufferOptions offscreenBufferOptions;
                offscreenBufferOptions.color = ftk::ImageType::RGBA_F32;
                if (ftk::gl::doCreate(p.buffer, size, offscreenBufferOptions))
                {
                    p.buffer = ftk::gl::OffscreenBuffer::create(size, offscreenBufferOptions);
                }

                const ftk::ViewportState viewportState(event.render);
                const ftk::ClipRectEnabledState clipRectEnabledState(event.render);
                const ftk::ClipRectState clipRectState(event.render);
                const ftk::TransformState transformState(event.render);
                const ftk::RenderSizeState renderSizeState(event.render);
                event.render->setClipRectEnabled(false);

                for (const auto& node : p.document->getGraph()->getLeafNodes())
                {
                    try
                    {
                        node->exec(event.render, p.currentTime);
                    }
                    catch (const std::exception& e)
                    {
                        if (auto context = getContext())
                        {
                            context->log("ibis::ui::Viewport", e.what(), ftk::LogType::Error);
                        }
                    }
                }

                if (p.buffer && p.node)
                {
                    ftk::gl::OffscreenBufferBinding binding(p.buffer);
                    event.render->setRenderSize(size);
                    event.render->setViewport(ftk::Box2I(0, 0, size.w, size.h));
                    event.render->clearViewport(ftk::Color4F(0.F, 0.F, 0.F, 0.F));
                    const auto pm = ftk::ortho(
                        0.F,
                        static_cast<float>(size.w),
                        static_cast<float>(size.h),
                        0.F,
                        -1.F,
                        1.F);
                    const ftk::M44F vm;
                    event.render->setTransform(pm * vm);

                    const auto& outputs = p.node->getOutputs();
                    if (!outputs.empty() && outputs.front())
                    {
                        const auto& output = outputs.front();
                        event.render->drawTexture(
                            output->getColorID(),
                            ftk::Box2I(ftk::V2I(0, 0), output->getSize()));
                    }
                }
                else if (!p.node)
                {
                    p.buffer.reset();
                }
            }
            event.render->drawRect(g, ftk::Color4F(0.F, 0.F, 0.F));
            if (p.buffer)
            {
                event.render->drawTexture(p.buffer->getColorID(), g, true);
            }
        }
    }
}
