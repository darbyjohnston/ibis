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
            std::shared_ptr<ftk::Observable<ftk::V2I> > viewPos;
            std::shared_ptr<ftk::Observable<double> > viewZoom;
            std::shared_ptr<ftk::Observable<std::pair<ftk::V2I, double> > > viewPosZoom;
            std::shared_ptr<ftk::Observable<bool> > frameView;
            std::shared_ptr<ftk::Observable<bool> > framed;
            float mouseWheelScale = 1.1F;

            int sizeHint = 0;
            bool doRender = true;
            std::shared_ptr<render::INode> node;
            OTIO_NS::RationalTime currentTime;
            std::shared_ptr<ftk::gl::OffscreenBuffer> buffer;

            std::shared_ptr<ftk::ListObserver<std::shared_ptr<render::INode> > > nodesObserver;
            std::shared_ptr<ftk::Observer<bool> > changedObserver;
            std::shared_ptr<ftk::Observer<OTIO_NS::RationalTime> > currentTimeObserver;
            std::shared_ptr<ftk::Observer<std::shared_ptr<render::INode> > > viewNodeObserver;

            enum class MouseMode
            {
                None,
                View
            };
            struct MouseData
            {
                bool inside = false;
                ftk::V2I press;
                MouseMode mode = MouseMode::None;
                ftk::V2I viewPos;
            };
            MouseData mouse;
        };

        void Viewport::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::ui::Viewport", parent);
            FTK_P();

            p.document = document;
            p.viewPos = ftk::Observable<ftk::V2I>::create();
            p.viewZoom = ftk::Observable<double>::create(1.0);
            p.viewPosZoom = ftk::Observable<std::pair<ftk::V2I, double> >::create(
                std::make_pair(ftk::V2I(), 1.0));
            p.frameView = ftk::Observable<bool>::create(true);
            p.framed = ftk::Observable<bool>::create(false);

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

        const ftk::V2I& Viewport::getViewPos() const
        {
            return _p->viewPos->get();
        }

        std::shared_ptr<ftk::IObservable<ftk::V2I> > Viewport::observeViewPos() const
        {
            return _p->viewPos;
        }

        double Viewport::getViewZoom() const
        {
            return _p->viewZoom->get();
        }

        std::shared_ptr<ftk::IObservable<double> > Viewport::observeViewZoom() const
        {
            return _p->viewZoom;
        }

        std::pair<ftk::V2I, double> Viewport::getViewPosAndZoom() const
        {
            return _p->viewPosZoom->get();
        }

        std::shared_ptr<ftk::IObservable<std::pair<ftk::V2I, double> > > Viewport::observeViewPosAndZoom() const
        {
            return _p->viewPosZoom;
        }

        void Viewport::setViewPosAndZoom(const ftk::V2I& pos, double zoom)
        {
            FTK_P();
            const std::pair<ftk::V2I, double> pair(pos, zoom);
            if (pair != p.viewPosZoom->get())
            {
                setFrameView(false);
            }
            if (p.viewPosZoom->setIfChanged(pair))
            {
                p.viewPos->setIfChanged(pos);
                p.viewZoom->setIfChanged(zoom);
                p.doRender = true;
                setDrawUpdate();
            }
        }

        void Viewport::setViewZoom(double zoom, const ftk::V2I& focus)
        {
            FTK_P();
            ftk::V2I pos;
            const ftk::V2I& viewPos = p.viewPos->get();
            const double viewZoom = p.viewZoom->get();
            pos.x = focus.x + (viewPos.x - focus.x) * (zoom / viewZoom);
            pos.y = focus.y + (viewPos.y - focus.y) * (zoom / viewZoom);
            setViewPosAndZoom(pos, zoom);
        }

        bool Viewport::hasFrameView() const
        {
            return _p->frameView->get();
        }

        std::shared_ptr<ftk::IObservable<bool> > Viewport::observeFrameView() const
        {
            return _p->frameView;
        }

        std::shared_ptr<ftk::IObservable<bool> > Viewport::observeFramed() const
        {
            return _p->framed;
        }

        void Viewport::setFrameView(bool value)
        {
            FTK_P();
            if (p.frameView->setIfChanged(value))
            {
                if (value)
                {
                    p.framed->setAlways(true);
                }
                p.doRender = true;
                setDrawUpdate();
            }
        }

        void Viewport::viewZoomReset()
        {
            FTK_P();
            setViewZoom(1.F, _getViewportCenter());
        }

        void Viewport::viewZoomIn()
        {
            FTK_P();
            setViewZoom(p.viewZoom->get() * 2.0, _getViewportCenter());
        }

        void Viewport::viewZoomOut()
        {
            FTK_P();
            setViewZoom(p.viewZoom->get() / 2.0, _getViewportCenter());
        }

        ftk::Color4F Viewport::getColorSample(const ftk::V2I& value)
        {
            FTK_P();
            ftk::Color4F out;
            if (p.buffer)
            {
                const ftk::Box2I& g = getGeometry();
                std::vector<float> sample(4);
                ftk::gl::OffscreenBufferBinding binding(p.buffer);
                glPixelStorei(GL_PACK_ALIGNMENT, 1);
#if defined(FTK_API_GL_4_1)
                glClampColor(GL_CLAMP_READ_COLOR, GL_FALSE);
#endif // FTK_API_GL_4_1
                glReadPixels(
                    value.x,
                    g.h() - 1 - value.y,
                    1,
                    1,
                    GL_RGBA,
                    GL_FLOAT,
                    sample.data());
                out.r = std::isnan(sample[0]) || std::isinf(sample[0]) ? 0.F : sample[0];
                out.g = std::isnan(sample[1]) || std::isinf(sample[1]) ? 0.F : sample[1];
                out.b = std::isnan(sample[2]) || std::isinf(sample[2]) ? 0.F : sample[2];
                out.a = std::isnan(sample[3]) || std::isinf(sample[3]) ? 0.F : sample[3];
            }
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
            }
        }

        void Viewport::setMouseWheelScale(float value)
        {
            _p->mouseWheelScale = value;
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

            if (p.doRender)
            {
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
            }

            if (p.frameView->get())
            {
                _frameView();
            }

            const ftk::Box2I& g = getGeometry();
            event.render->drawRect(g, ftk::Color4F(0.F, 0.F, 0.F));

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
                ftk::gl::OffscreenBufferBinding binding(p.buffer);

                const ftk::ViewportState viewportState(event.render);
                const ftk::ClipRectEnabledState clipRectEnabledState(event.render);
                const ftk::ClipRectState clipRectState(event.render);
                const ftk::TransformState transformState(event.render);
                const ftk::RenderSizeState renderSizeState(event.render);
                event.render->setClipRectEnabled(false);
                event.render->setRenderSize(size);
                event.render->setViewport(ftk::Box2I(0, 0, size.w, size.h));
                event.render->clearViewport(ftk::Color4F(0.F, 0.F, 0.F, 0.F));

                if (p.buffer && p.node)
                {
                    const auto pm = ftk::ortho(
                        0.F,
                        static_cast<float>(size.w),
                        static_cast<float>(size.h),
                        0.F,
                        -1.F,
                        1.F);
                    const ftk::V2I& viewPos = p.viewPos->get();
                    const double viewZoom = p.viewZoom->get();
                    const ftk::M44F vm =
                        ftk::translate(ftk::V3F(viewPos.x, viewPos.y, 0.F)) *
                        ftk::scale(ftk::V3F(viewZoom, viewZoom, 1.F));
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
            if (p.buffer)
            {
                event.render->drawTexture(p.buffer->getColorID(), g, true);
            }
        }

        void Viewport::mouseEnterEvent(ftk::MouseEnterEvent& event)
        {
            FTK_P();
            event.accept = true;
            p.mouse.inside = true;
        }

        void Viewport::mouseLeaveEvent()
        {
            FTK_P();
            p.mouse.inside = false;
        }

        void Viewport::mouseMoveEvent(ftk::MouseMoveEvent& event)
        {
            FTK_P();
            event.accept = true;

            const ftk::Box2I& g = getGeometry();
            const ftk::V2I pos(
                event.pos.x - g.min.x,
                event.pos.y - g.min.y);

            switch (p.mouse.mode)
            {
            case Private::MouseMode::View:
            {
                const ftk::V2I viewPos(
                    p.mouse.viewPos.x + (pos.x - p.mouse.press.x),
                    p.mouse.viewPos.y + (pos.y - p.mouse.press.y));
                const double viewZoom = p.viewZoom->get();
                const std::pair<ftk::V2I, double> pair(viewPos, viewZoom);
                if (pair != p.viewPosZoom->get())
                {
                    setFrameView(false);
                }
                if (p.viewPosZoom->setIfChanged(std::make_pair(viewPos, viewZoom)))
                {
                    p.viewPos->setIfChanged(viewPos);
                    p.viewZoom->setIfChanged(viewZoom);
                    p.doRender = true;
                    setDrawUpdate();
                }
                break;
            }
            default: break;
            }
        }

        void Viewport::mousePressEvent(ftk::MouseClickEvent& event)
        {
            FTK_P();
            event.accept = true;
            takeKeyFocus();

            const ftk::Box2I& g = getGeometry();
            const ftk::V2I pos(
                event.pos.x - g.min.x,
                event.pos.y - g.min.y);
            p.mouse.press = pos;

            if (ftk::MouseButton::Middle == event.button &&
                0 == event.modifiers)
            {
                p.mouse.mode = Private::MouseMode::View;
                p.mouse.viewPos = p.viewPos->get();
            }
            else
            {
                p.mouse.mode = Private::MouseMode::None;
            }
        }

        void Viewport::mouseReleaseEvent(ftk::MouseClickEvent& event)
        {
            FTK_P();
            event.accept = true;
            p.mouse.mode = Private::MouseMode::None;
        }

        void Viewport::scrollEvent(ftk::ScrollEvent& event)
        {
            FTK_P();
            if (static_cast<int>(ftk::KeyModifier::None) == event.modifiers)
            {
                event.accept = true;

                const ftk::Box2I& g = getGeometry();
                const ftk::V2I pos(
                    event.pos.x - g.min.x,
                    event.pos.y - g.min.y);

                const double viewZoom = p.viewZoom->get();
                const double newZoom =
                    event.value.y > 0 ?
                    viewZoom * p.mouseWheelScale :
                    viewZoom / p.mouseWheelScale;
                setViewZoom(newZoom, pos);
            }
        }

        void Viewport::keyPressEvent(ftk::KeyEvent& event)
        {
            FTK_P();
            const ftk::Box2I& g = getGeometry();
            const ftk::V2I pos(
                event.pos.x - g.min.x,
                event.pos.y - g.min.y);

            if (0 == event.modifiers)
            {
                switch (event.key)
                {
                case ftk::Key::_0:
                    event.accept = true;
                    setViewZoom(1.0, pos);
                    break;
                case ftk::Key::Equals:
                    event.accept = true;
                    setViewZoom(p.viewZoom->get() * 2.0, pos);
                    break;
                case ftk::Key::Minus:
                    event.accept = true;
                    setViewZoom(p.viewZoom->get() / 2.0, pos);
                    break;
                case ftk::Key::Backspace:
                    event.accept = true;
                    setFrameView(true);
                    break;
                default: break;
                }
            }
        }

        void Viewport::keyReleaseEvent(ftk::KeyEvent& event)
        {
            event.accept = true;
        }

        bool Viewport::_isMouseInside() const
        {
            return _p->mouse.inside;
        }

        const ftk::V2I& Viewport::_getMousePressPos() const
        {
            return _p->mouse.press;
        }

        ftk::V2I Viewport::_getViewportCenter() const
        {
            const ftk::Box2I& g = getGeometry();
            return ftk::V2I(g.w() / 2, g.h() / 2);
        }

        void Viewport::_frameView()
        {
            FTK_P();
            if (p.node &&
                !p.node->getOutputs().empty() &&
                p.node->getOutputs().front())
            {
                const auto& output = p.node->getOutputs().front();
                ftk::V2I viewPos;
                double viewZoom = 1.0;
                const ftk::Box2I& g = getGeometry();
                const ftk::Size2I viewportSize = g.size();
                const ftk::Size2I renderSize = output->getSize();
                if (renderSize.w > 0 && renderSize.h > 0)
                {
                    viewZoom = viewportSize.w / static_cast<double>(renderSize.w);
                    if (viewZoom * renderSize.h > viewportSize.h)
                    {
                        viewZoom = viewportSize.h / static_cast<double>(renderSize.h);
                    }
                    const ftk::V2I c(renderSize.w / 2, renderSize.h / 2);
                    viewPos = ftk::V2I(
                        viewportSize.w / 2.F - c.x * viewZoom,
                        viewportSize.h / 2.F - c.y * viewZoom);
                }
                if (p.viewPosZoom->setIfChanged(std::make_pair(viewPos, viewZoom)))
                {
                    p.viewPos->setIfChanged(viewPos);
                    p.viewZoom->setIfChanged(viewZoom);
                }
            }
        }
    }
}
