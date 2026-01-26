// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "Viewport.h"

#include <ibis/Models/Document.h>

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

            int sizeHint = 0;

            bool doRender = true;
            std::shared_ptr<ftk::gl::OffscreenBuffer> buffer;

            std::shared_ptr<ftk::ListObserver<std::shared_ptr<render::INode> > > nodesObserver;
            std::shared_ptr<ftk::Observer<bool> > changedObserver;
            std::shared_ptr<ftk::ListObserver<std::shared_ptr<render::INode> > > selectionObserver;
        };

        void Viewport::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::ui::Viewport", parent);
            FTK_P();

            p.document = document;

            _nodeUpdate();

            p.nodesObserver = ftk::ListObserver<std::shared_ptr<render::INode> >::create(
                document->getGraph()->observeNodes(),
                [this](const std::vector<std::shared_ptr<render::INode> >& nodes)
                {
                    FTK_P();
                    const auto i = std::find(nodes.begin(), nodes.end(), p.node);
                    if (i == nodes.end())
                    {
                        p.node.reset();
                        _nodeUpdate();
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

            p.selectionObserver = ftk::ListObserver<std::shared_ptr<render::INode> >::create(
                document->observeSelection(),
                [this](const std::vector<std::shared_ptr<render::INode> >& selection)
                {
                    FTK_P();
                    if (!selection.empty() && selection.front() != p.node)
                    {
                        p.node = selection.front();
                        p.doRender = true;
                        setDrawUpdate();
                    }
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
            const ftk::Size2I size = g.size();
            if (p.doRender)
            {
                p.doRender = false;
                if (p.node)
                {
                    try
                    {
                        ftk::gl::OffscreenBufferOptions offscreenBufferOptions;
                        offscreenBufferOptions.color = ftk::ImageType::RGBA_F32;
                        if (ftk::gl::doCreate(p.buffer, size, offscreenBufferOptions))
                        {
                            p.buffer = ftk::gl::OffscreenBuffer::create(size, offscreenBufferOptions);
                        }
                        ftk::gl::OffscreenBufferBinding binding(p.buffer);

                        const ftk::ViewportState viewportState(event.render);
                        const ftk::ClipRectEnabledState clipRectEnabledState(event.render);
                        const ftk::ClipRectState clipRectState(event.render);
                        const ftk::TransformState transformState(event.render);
                        const ftk::RenderSizeState renderSizeState(event.render);
                        event.render->setClipRectEnabled(false);

                        p.node->exec(event.render);
                        const auto& outputs = p.node->getOutputs();
                        if (!outputs.empty() && outputs.front())
                        {
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

                            const auto& output = outputs.front();
                            event.render->drawTexture(
                                output->getColorID(),
                                ftk::Box2I(ftk::V2I(0, 0), output->getSize()));
                        }
                    }
                    catch (const std::exception& e)
                    {
                        if (auto context = getContext())
                        {
                            context->log("ibis::ui::Viewport", e.what(), ftk::LogType::Error);
                        }
                    }
                }
                else
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

        void Viewport::_nodeUpdate()
        {
            FTK_P();
            if (!p.node)
            {
                const auto& selection = p.document->getSelection();
                if (!selection.empty())
                {
                    p.node = selection.front();
                }
                else
                {
                    const auto leafNodes = p.document->getGraph()->getLeafNodes();
                    if (!leafNodes.empty())
                    {
                        p.node = leafNodes.front();
                    }
                }
                if (p.node)
                {
                    p.doRender = true;
                    setDrawUpdate();
                }
            }
        }
    }
}
