// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "Viewport.h"

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
            std::shared_ptr<render::Graph> graph;

            int sizeHint = 0;

            bool doRender = true;
            std::shared_ptr<ftk::gl::OffscreenBuffer> buffer;

            std::shared_ptr<ftk::Observer<bool> > changedObserver;
        };

        void Viewport::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<render::Graph>& graph,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::ui::Viewport", parent);
            FTK_P();
            p.graph = graph;

            p.changedObserver = ftk::Observer<bool>::create(
                graph->observe(),
                [this](bool value)
                {
                    if (value)
                    {
                        _p->doRender = true;
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
            const std::shared_ptr<render::Graph>& graph,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<Viewport> out(new Viewport);
            out->_init(context, graph, parent);
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

                    const auto nodes = p.graph->getLeafNodes();
                    if (!nodes.empty())
                    {
                        auto& node = nodes.front();
                        node->exec(event.render);
                        const auto& outputs = node->getOutputs();
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
                }
                catch (const std::exception& e)
                {
                    if (auto context = getContext())
                    {
                        context->log("ibis::ui::Viewport", e.what(), ftk::LogType::Error);
                    }
                }
            }
            if (p.buffer)
            {
                event.render->drawRect(g, ftk::Color4F(0.F, 0.F, 0.F));
                event.render->drawTexture(p.buffer->getColorID(), g, true);
            }
        }
    }
}
