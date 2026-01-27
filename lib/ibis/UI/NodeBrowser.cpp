// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "NodeBrowser.h"

#include <ftk/UI/Label.h>
#include <ftk/UI/RowLayout.h>
#include <ftk/UI/ScrollWidget.h>

namespace ibis
{
    namespace ui
    {
        struct NodeBrowserItem::Private
        {
            std::string node;
            std::shared_ptr<ftk::Label> label;

            int dragLength = 0;
            float iconScale = 1.F;
            std::shared_ptr<ftk::Image> dragImage;
        };

        void NodeBrowserItem::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::string& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::NodeBrowserItem", parent);
            FTK_P();

            _setMouseHoverEnabled(true);
            _setMousePressEnabled(true);

            p.node = node;

            p.label = ftk::Label::create(context, node, shared_from_this());
            p.label->setMarginRole(ftk::SizeRole::MarginSmall);
        }

        NodeBrowserItem::NodeBrowserItem() :
            _p(new Private)
        {}

        NodeBrowserItem::~NodeBrowserItem()
        {}

        std::shared_ptr<NodeBrowserItem> NodeBrowserItem::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::string& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<NodeBrowserItem> out(new NodeBrowserItem);
            out->_init(context, node, parent);
            return out;
        }

        ftk::Size2I NodeBrowserItem::getSizeHint() const
        {
            return _p->label->getSizeHint();
        }

        void NodeBrowserItem::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->label->setGeometry(value);
        }

        void NodeBrowserItem::sizeHintEvent(const ftk::SizeHintEvent& event)
        {
            IMouseWidget::sizeHintEvent(event);
            FTK_P();
            p.dragLength = event.style->getSizeRole(ftk::SizeRole::DragLength, event.displayScale);
            if (event.displayScale != p.iconScale)
            {
                p.iconScale = event.displayScale;
                p.dragImage.reset();
            }
            if (!p.dragImage)
            {
                p.dragImage = event.iconSystem->get("NodeDragDrop", event.displayScale);
            }
        }

        void NodeBrowserItem::drawEvent(
            const ftk::Box2I& drawRect,
            const ftk::DrawEvent& event)
        {
            IMouseWidget::drawEvent(drawRect, event);
            const ftk::Box2I& g = getGeometry();
            if (_isMouseInside())
            {
                event.render->drawRect(
                    g,
                    event.style->getColorRole(ftk::ColorRole::Hover));
            }
        }

        void NodeBrowserItem::mouseEnterEvent(ftk::MouseEnterEvent& event)
        {
            IMouseWidget::mouseEnterEvent(event);
            setDrawUpdate();
        }

        void NodeBrowserItem::mouseLeaveEvent()
        {
            IMouseWidget::mouseLeaveEvent();
            setDrawUpdate();
        }

        void NodeBrowserItem::mouseMoveEvent(ftk::MouseMoveEvent& event)
        {
            IMouseWidget::mouseMoveEvent(event);
            FTK_P();
            if (_isMousePressed() && p.dragImage)
            {
                const float length = ftk::length(event.pos - _getMousePressPos());
                if (length > p.dragLength)
                {
                    event.dragDropData = std::make_shared<NodeDragDropData>(p.node);
                    event.dragDropCursor = p.dragImage;
                    const ftk::Box2I& g = getGeometry();
                    event.dragDropCursorHotspot.x = p.dragImage->getWidth() / 2;
                    event.dragDropCursorHotspot.y = p.dragImage->getHeight() / 2;
                }
            }
        }

        struct NodeBrowser::Private
        {
            std::shared_ptr<ftk::VerticalLayout> layout;
            std::shared_ptr<ftk::ScrollWidget> scrollWidget;
        };

        void NodeBrowser::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<render::NodeFactory>& factory,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::NodeBrowser", parent);
            FTK_P();

            p.layout = ftk::VerticalLayout::create(context);
            p.layout->setSpacingRole(ftk::SizeRole::None);
            for (const auto& node : factory->getIDs())
            {
                auto item = NodeBrowserItem::create(context, node, p.layout);
            }

            p.scrollWidget = ftk::ScrollWidget::create(context, ftk::ScrollType::Both, shared_from_this());
            p.scrollWidget->setBorder(false);
            p.scrollWidget->setWidget(p.layout);
        }

        NodeBrowser::NodeBrowser() :
            _p(new Private)
        {}

        NodeBrowser::~NodeBrowser()
        {}

        std::shared_ptr<NodeBrowser> NodeBrowser::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<render::NodeFactory>& factory,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<NodeBrowser> out(new NodeBrowser);
            out->_init(context, factory, parent);
            return out;
        }

        ftk::Size2I NodeBrowser::getSizeHint() const
        {
            return _p->scrollWidget->getSizeHint();
        }

        void NodeBrowser::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->scrollWidget->setGeometry(value);
        }
    }
}
