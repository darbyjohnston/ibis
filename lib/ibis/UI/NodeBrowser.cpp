// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "NodeBrowser.h"

#include <ftk/UI/Bellows.h>
#include <ftk/UI/Divider.h>
#include <ftk/UI/Label.h>
#include <ftk/UI/RowLayout.h>
#include <ftk/UI/ScrollWidget.h>
#include <ftk/UI/SearchBox.h>
#include <ftk/UI/ToolButton.h>
#include <ftk/Core/String.h>

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
            std::vector<std::string> nodeGroups;
            std::map<std::string, render::NodeInfo> nodeInfo;
            std::map<std::string, std::shared_ptr<NodeBrowserItem> > items;
            std::map<std::string, std::shared_ptr<ftk::Bellows> > bellows;
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

            p.nodeGroups = factory->getGroups();
            p.nodeInfo = factory->getInfo();

            auto vLayout = ftk::VerticalLayout::create(context);
            vLayout->setSpacingRole(ftk::SizeRole::None);
            for (const auto& group : p.nodeGroups)
            {
                auto bellows = ftk::Bellows::create(context, group, vLayout);
                p.bellows[group] = bellows;
                auto groupLayout = ftk::VerticalLayout::create(context);
                groupLayout->setSpacingRole(ftk::SizeRole::None);
                bellows->setWidget(groupLayout);
                for (const auto& node : factory->getIDs(group))
                {
                    p.items[node] = NodeBrowserItem::create(
                        context,
                        factory->getInfo(node),
                        groupLayout);
                }
            }

            auto scrollWidget = ftk::ScrollWidget::create(context, ftk::ScrollType::Both);
            scrollWidget->setBorder(false);
            scrollWidget->setVStretch(ftk::Stretch::Expanding);
            scrollWidget->setWidget(vLayout);

            auto expandAllButton = ftk::ToolButton::create(context);
            expandAllButton->setIcon("BellowsOpen");
            expandAllButton->setTooltip("Expand all groups.");
            auto closeAllButton = ftk::ToolButton::create(context);
            closeAllButton->setIcon("BellowsClosed");
            closeAllButton->setTooltip("Close all groups.");

            p.searchBox = ftk::SearchBox::create(context);
            p.searchBox->setHStretch(ftk::Stretch::Expanding);

            p.layout = ftk::VerticalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(ftk::SizeRole::None);
            scrollWidget->setParent(p.layout);
            ftk::Divider::create(context, ftk::Orientation::Vertical, p.layout);
            auto hLayout = ftk::HorizontalLayout::create(context, p.layout);
            hLayout->setMarginRole(ftk::SizeRole::MarginInside);
            hLayout->setSpacingRole(ftk::SizeRole::SpacingTool);
            expandAllButton->setParent(hLayout);
            closeAllButton->setParent(hLayout);
            p.searchBox->setParent(hLayout);

            expandAllButton->setClickedCallback(
                [this]
                {
                    FTK_P();
                    for (const auto i : p.bellows)
                    {
                        i.second->setOpen(true);
                    }
                });
            closeAllButton->setClickedCallback(
                [this]
                {
                    FTK_P();
                    for (const auto i : p.bellows)
                    {
                        i.second->setOpen(false);
                    }
                });

            p.searchBox->setCallback(
                [this](const std::string& value)
                {
                    _searchUpdate(value);
                });
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

        void NodeBrowser::_searchUpdate(const std::string& value)
        {
            FTK_P();
            std::map<std::string, bool> bellows;
            for (const auto& group : p.nodeGroups)
            {
                bellows[group] = false;
            }

            for (const auto& i : p.nodeInfo)
            {
                const bool match = ftk::contains(
                    i.second.name,
                    value,
                    ftk::CaseCompare::Insensitive);

                const auto j = p.items.find(i.first);
                if (j != p.items.end())
                {
                    j->second->setVisible(match);
                }

                const auto k = bellows.find(i.second.group);
                if (k != bellows.end())
                {
                    k->second = k->second || match;
                }
            }

            for (const auto& i : bellows)
            {
                const auto j = p.bellows.find(i.first);
                if (j != p.bellows.end())
                {
                    j->second->setOpen(i.second);
                    j->second->setVisible(i.second);
                }
            }
        }
    }
}
