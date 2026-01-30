// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "NodeBrowser.h"

#include <ftk/UI/Divider.h>
#include <ftk/UI/Label.h>
#include <ftk/UI/RowLayout.h>
#include <ftk/UI/ScrollWidget.h>
#include <ftk/UI/SearchBox.h>

namespace ibis
{
    namespace ui
    {
        struct NodeBrowserItem::Private
        {
            render::NodeInfo info;
            std::shared_ptr<ftk::Label> label;
            std::shared_ptr<ftk::HorizontalLayout> layout;

            int dragLength = 0;
            float iconScale = 1.F;
            std::shared_ptr<ftk::Image> dragImage;
        };

        void NodeBrowserItem::_init(
            const std::shared_ptr<ftk::Context>& context,
            const render::NodeInfo& info,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::NodeBrowserItem", parent);
            FTK_P();

            _setMouseHoverEnabled(true);
            _setMousePressEnabled(true);

            p.info = info;

            p.label = ftk::Label::create(context, info.name);
            p.label->setMarginRole(ftk::SizeRole::MarginSmall);

            p.layout = ftk::HorizontalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(ftk::SizeRole::SpacingSmall);
            p.label->setParent(p.layout);
        }

        NodeBrowserItem::NodeBrowserItem() :
            _p(new Private)
        {}

        NodeBrowserItem::~NodeBrowserItem()
        {}

        std::shared_ptr<NodeBrowserItem> NodeBrowserItem::create(
            const std::shared_ptr<ftk::Context>& context,
            const render::NodeInfo& info,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<NodeBrowserItem> out(new NodeBrowserItem);
            out->_init(context, info, parent);
            return out;
        }

        ftk::Size2I NodeBrowserItem::getSizeHint() const
        {
            return _p->layout->getSizeHint();
        }

        void NodeBrowserItem::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
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
                    event.dragDropData = std::make_shared<NodeDragDropData>(p.info.id);
                    event.dragDropCursor = p.dragImage;
                    const ftk::Box2I& g = getGeometry();
                    event.dragDropCursorHotspot.x = p.dragImage->getWidth() / 2;
                    event.dragDropCursorHotspot.y = p.dragImage->getHeight() / 2;
                }
            }
        }

        struct NodeBrowser::Private
        {
            std::shared_ptr<ftk::VerticalLayout> itemLayout;
            std::shared_ptr<ftk::ScrollWidget> scrollWidget;
            std::shared_ptr<ftk::SearchBox> searchBox;
            std::shared_ptr<ftk::VerticalLayout> layout;
        };

        void NodeBrowser::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<render::NodeFactory>& factory,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::NodeBrowser", parent);
            FTK_P();

            p.itemLayout = ftk::VerticalLayout::create(context);
            p.itemLayout->setSpacingRole(ftk::SizeRole::None);
            for (const auto& info : factory->getInfo())
            {
                auto item = NodeBrowserItem::create(context, info, p.itemLayout);
            }

            p.scrollWidget = ftk::ScrollWidget::create(context, ftk::ScrollType::Both);
            p.scrollWidget->setBorder(false);
            p.scrollWidget->setVStretch(ftk::Stretch::Expanding);
            p.scrollWidget->setWidget(p.itemLayout);

            p.searchBox = ftk::SearchBox::create(context);
            p.searchBox->setHStretch(ftk::Stretch::Expanding);

            p.layout = ftk::VerticalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(ftk::SizeRole::None);
            p.scrollWidget->setParent(p.layout);
            ftk::Divider::create(context, ftk::Orientation::Vertical, p.layout);
            auto hLayout = ftk::HorizontalLayout::create(context, p.layout);
            hLayout->setMarginRole(ftk::SizeRole::MarginInside);
            hLayout->setSpacingRole(ftk::SizeRole::SpacingSmall);
            p.searchBox->setParent(hLayout);
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
            return _p->layout->getSizeHint();
        }

        void NodeBrowser::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }
    }
}
