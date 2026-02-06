// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include <ibis/UI/NodeMiniMap.h>

#include <ftk/UI/DrawUtil.h>
#include <ftk/UI/IMouseWidget.h>

#include <optional>

namespace ibis
{
    namespace ui
    {
        namespace
        {
            class FTK_API_TYPE NodeMiniMapWidget : public ftk::IMouseWidget
            {
            protected:
                void _init(
                    const std::shared_ptr<ftk::Context>&,
                    const std::shared_ptr<ftk::IWidget>& parent);

                NodeMiniMapWidget() = default;

            public:
                virtual ~NodeMiniMapWidget() {}

                static std::shared_ptr<NodeMiniMapWidget> create(
                    const std::shared_ptr<ftk::Context>&,
                    const std::shared_ptr<ftk::IWidget>& parent = nullptr);

                void setScrollInfo(const ftk::ScrollInfo&);
                void setCallback(const std::function<void(const ftk::V2I&)>&);
                void setChildGeometry(const std::vector<ftk::Box2I>&);
                ftk::ColorRole getChildColorRole() const;
                void setChildColorRole(ftk::ColorRole);

                ftk::Size2I getSizeHint() const override;
                void sizeHintEvent(const ftk::SizeHintEvent&) override;
                void drawEvent(const ftk::Box2I&, const ftk::DrawEvent&) override;
                void mouseMoveEvent(ftk::MouseMoveEvent&) override;
                void mousePressEvent(ftk::MouseClickEvent&) override;

            private:
                ftk::ScrollInfo _scrollInfo;
                std::function<void(const ftk::V2I&)> _callback;
                std::vector<ftk::Box2I> _childGeometry;
                ftk::ColorRole _childColorRole = ftk::ColorRole::Cyan;

                ftk::V2I _scrollPosPress;

                struct SizeData
                {
                    std::optional<float> displayScale;
                    int sizeHint = 0;
                    int border = 0;
                };
                SizeData _size;
            };

            void NodeMiniMapWidget::_init(
                const std::shared_ptr<ftk::Context>& context,
                const std::shared_ptr<IWidget>& parent)
            {
                IMouseWidget::_init(context, "ibis::ui::NodeMiniMapWidget", parent);
                setBackgroundRole(ftk::ColorRole::Overlay);
                _setMouseHoverEnabled(true);
                _setMousePressEnabled(true);
            }

            std::shared_ptr<NodeMiniMapWidget> NodeMiniMapWidget::create(
                const std::shared_ptr<ftk::Context>& context,
                const std::shared_ptr<IWidget>& parent)
            {
                auto out = std::shared_ptr<NodeMiniMapWidget>(new NodeMiniMapWidget);
                out->_init(context, parent);
                return out;
            }

            void NodeMiniMapWidget::setScrollInfo(const ftk::ScrollInfo& value)
            {
                if (value == _scrollInfo)
                    return;
                _scrollInfo = value;
                setSizeUpdate();
                setDrawUpdate();
            }

            void NodeMiniMapWidget::setCallback(const std::function<void(const ftk::V2I&)>& value)
            {
                _callback = value;
            }

            void NodeMiniMapWidget::setChildGeometry(const std::vector<ftk::Box2I>& value)
            {
                if (value == _childGeometry)
                    return;
                _childGeometry = value;
                setDrawUpdate();
            }

            ftk::ColorRole NodeMiniMapWidget::getChildColorRole() const
            {
                return _childColorRole;
            }

            void NodeMiniMapWidget::setChildColorRole(ftk::ColorRole value)
            {
                if (value == _childColorRole)
                    return;
                _childColorRole = value;
                setDrawUpdate();
            }

            ftk::Size2I NodeMiniMapWidget::getSizeHint() const
            {
                float aspect = 1.F;
                if (_scrollInfo.scrollSize.isValid())
                {
                    aspect = _scrollInfo.scrollSize.w / static_cast<float>(_scrollInfo.scrollSize.h);
                }
                return ftk::Size2I(_size.sizeHint * aspect, _size.sizeHint) + _size.border * 2;
            }

            void NodeMiniMapWidget::sizeHintEvent(const ftk::SizeHintEvent& event)
            {
                IMouseWidget::sizeHintEvent(event);
                if (!_size.displayScale.has_value() ||
                    (_size.displayScale.has_value() && _size.displayScale.value() != event.displayScale))
                {
                    _size.displayScale = event.displayScale;
                    _size.sizeHint = event.style->getSizeRole(ftk::SizeRole::ScrollArea, event.displayScale) / 2;
                    _size.border = event.style->getSizeRole(ftk::SizeRole::Border, event.displayScale);
                }
            }

            void NodeMiniMapWidget::drawEvent(
                const ftk::Box2I& drawRect,
                const ftk::DrawEvent& event)
            {
                IMouseWidget::drawEvent(drawRect, event);

                const ftk::Box2I& g = getGeometry();
                event.render->drawMesh(
                    border(g, _size.border),
                    event.style->getColorRole(ftk::ColorRole::Border));

                if (_scrollInfo.scrollSize.isValid())
                {
                    std::vector<ftk::Box2I> rects;
                    const ftk::Box2I g2 = margin(g, -_size.border);
                    const float sw = static_cast<float>(_scrollInfo.scrollSize.w);
                    const float sh = static_cast<float>(_scrollInfo.scrollSize.h);
                    for (const auto& cg : _childGeometry)
                    {
                        rects.push_back(ftk::Box2I(
                            g2.min.x + cg.min.x / sw * g2.w(),
                            g2.min.y + cg.min.y / sh * g2.h(),
                            std::ceil(cg.w() / sw * g2.w()),
                            std::ceil(cg.h() / sh * g2.h())));
                    }
                    event.render->drawRects(
                        rects,
                        event.style->getColorRole(_childColorRole));

                    const ftk::Box2I viewport(
                        g2.min.x + _scrollInfo.scrollPos.x / sw * g2.w(),
                        g2.min.y + _scrollInfo.scrollPos.y / sh * g2.h(),
                        std::ceil(_scrollInfo.viewport.size().w / sw * g2.w()),
                        std::ceil(_scrollInfo.viewport.size().h / sh * g2.h()));
                    event.render->drawMesh(
                        border(viewport, _size.border),
                        event.style->getColorRole(ftk::ColorRole::Text));
                }
            }

            void NodeMiniMapWidget::mouseMoveEvent(ftk::MouseMoveEvent& event)
            {
                IMouseWidget::mouseMoveEvent(event);
                if (_isMousePressed() && _scrollInfo.scrollSize.isValid() && _callback)
                {
                    const ftk::Box2I& g = getGeometry();
                    const ftk::Box2I g2 = ftk::margin(g, -_size.border);
                    const ftk::V2I& m = _getMousePos();
                    const ftk::V2I& mp = _getMousePressPos();
                    const ftk::V2I v(
                        (m.x - mp.x) / static_cast<float>(g2.w()) * _scrollInfo.scrollSize.w,
                        (m.y - mp.y) / static_cast<float>(g2.h()) * _scrollInfo.scrollSize.h);
                    _callback(_scrollPosPress + v);
                }
            }

            void NodeMiniMapWidget::mousePressEvent(ftk::MouseClickEvent& event)
            {
                IMouseWidget::mousePressEvent(event);
                _scrollPosPress = _scrollInfo.scrollPos;
            }
        }

        struct NodeMiniMap::Private
        {
            std::shared_ptr<NodeMiniMapWidget> widget;

            struct SizeData
            {
                std::optional<float> displayScale;
                int margin = 0;
                int shadow = 0;
            };
            SizeData size;

            struct DrawData
            {
                ftk::TriMesh2F shadow;
            };
            std::optional<DrawData> draw;
        };

        void NodeMiniMap::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::ui::NodeMiniMap", parent);
            FTK_P();
            p.widget = NodeMiniMapWidget::create(context, shared_from_this());
        }

        NodeMiniMap::NodeMiniMap() :
            _p(new Private)
        {}

        NodeMiniMap::~NodeMiniMap()
        {}

        std::shared_ptr<NodeMiniMap> NodeMiniMap::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<NodeMiniMap>(new NodeMiniMap);
            out->_init(context, parent);
            return out;
        }

        void NodeMiniMap::setScrollInfo(const ftk::ScrollInfo& value)
        {
            _p->widget->setScrollInfo(value);
        }

        void NodeMiniMap::setCallback(const std::function<void(const ftk::V2I&)>& value)
        {
            _p->widget->setCallback(value);
        }

        void NodeMiniMap::setChildGeometry(const std::vector<ftk::Box2I>& value)
        {
            _p->widget->setChildGeometry(value);
        }

        ftk::ColorRole NodeMiniMap::getChildColorRole() const
        {
            return _p->widget->getChildColorRole();
        }

        void NodeMiniMap::setChildColorRole(ftk::ColorRole value)
        {
            _p->widget->setChildColorRole(value);
        }

        void NodeMiniMap::setGeometry(const ftk::Box2I& value)
        {
            const bool changed = value != getGeometry();
            IWidget::setGeometry(value);
            FTK_P();
            const ftk::Size2I sizeHint = p.widget->getSizeHint();
            p.widget->setGeometry(ftk::Box2I(
                value.max.x - sizeHint.w - p.size.margin,
                value.max.y - sizeHint.h - p.size.margin,
                sizeHint.w,
                sizeHint.h));
            if (changed)
            {
                p.draw.reset();
            }
        }

        void NodeMiniMap::sizeHintEvent(const ftk::SizeHintEvent& event)
        {
            FTK_P();
            if (!p.size.displayScale.has_value() ||
                (p.size.displayScale.has_value() && p.size.displayScale.value() != event.displayScale))
            {
                p.size.displayScale = event.displayScale;
                p.size.margin = event.style->getSizeRole(ftk::SizeRole::Margin, event.displayScale);
                p.size.shadow = event.style->getSizeRole(ftk::SizeRole::Shadow, event.displayScale);
            }
        }

        void NodeMiniMap::drawEvent(
            const ftk::Box2I& drawRect,
            const ftk::DrawEvent& event)
        {
            IWidget::drawEvent(drawRect, event);
            FTK_P();

            if (!p.draw.has_value())
            {
                p.draw = Private::DrawData();
                const ftk::Box2I& g = p.widget->getGeometry();
                p.draw->shadow = ftk::shadow(ftk::margin(g, p.size.shadow, 0, p.size.shadow, p.size.shadow), p.size.shadow);
            }

            event.render->drawColorMesh(p.draw->shadow);
        }
    }
}
