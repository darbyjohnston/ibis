// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "NodeGraphWidget.h"

#include "NodeWidgetFactory.h"

#include <ibis/Render/INode.h>
#include <ibis/Render/RenderUtil.h>

#include <ftk/UI/DrawUtil.h>
#include <ftk/UI/Label.h>
#include <ftk/UI/Icon.h>
#include <ftk/UI/Menu.h>
#include <ftk/UI/RowLayout.h>
#include <ftk/UI/ToolButton.h>
#include <ftk/GL/GL.h>
#include <ftk/Core/Format.h>
#include <ftk/Core/String.h>

namespace ibis
{
    namespace ui
    {
        struct INodeGraphPort::Private
        {
            std::shared_ptr<ftk::Icon> icon;
            std::shared_ptr<ftk::Icon> connectIcon;
        };

        void INodeGraphPort::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IMouseWidget::_init(context, "ibis::INodeGraphPort", parent);
            FTK_P();

            _setMouseHoverEnabled(true);

            p.icon = ftk::Icon::create(context, "NodePort", shared_from_this());

            p.connectIcon = ftk::Icon::create(context, "NodePortConnected", shared_from_this());
            p.connectIcon->hide();
        }

        INodeGraphPort::INodeGraphPort() :
            _p(new Private)
        {}

        INodeGraphPort::~INodeGraphPort()
        {}

        void INodeGraphPort::setConnect(bool value)
        {
            FTK_P();
            p.icon->setVisible(!value);
            p.connectIcon->setVisible(value);
        }

        ftk::Size2I INodeGraphPort::getSizeHint() const
        {
            FTK_P();
            return _p->icon->getSizeHint();
        }

        void INodeGraphPort::setGeometry(const ftk::Box2I& value)
        {
            IMouseWidget::setGeometry(value);
            FTK_P();
            p.icon->setGeometry(value);
            p.connectIcon->setGeometry(value);
        }

        void INodeGraphPort::sizeHintEvent(const ftk::SizeHintEvent& event)
        {}
        
        void INodeGraphPort::drawEvent(const ftk::Box2I& drawRect, const ftk::DrawEvent& event)
        {
            FTK_P();
            IMouseWidget::drawEvent(drawRect, event);
            if (_isMouseInside())
            {
                event.render->drawRect(
                    getGeometry(),
                    event.style->getColorRole(ftk::ColorRole::Hover));
            }
        }

        void INodeGraphPort::mouseEnterEvent(ftk::MouseEnterEvent& event)
        {
            IMouseWidget::mouseEnterEvent(event);
            setDrawUpdate();
        }

        void INodeGraphPort::mouseLeaveEvent()
        {
            IMouseWidget::mouseLeaveEvent();
            setDrawUpdate();
        }

        NodeGraphInput::~NodeGraphInput()
        {}

        std::shared_ptr<NodeGraphInput> NodeGraphInput::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<NodeGraphInput> out(new NodeGraphInput);
            out->_init(context, node, parent);
            return out;
        }

        NodeGraphOutput::~NodeGraphOutput()
        {}

        std::shared_ptr<NodeGraphOutput> NodeGraphOutput::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<NodeGraphOutput> out(new NodeGraphOutput);
            out->_init(context, node, parent);
            return out;
        }

        struct NodeGraphThumbnail::Private
        {
            std::shared_ptr<render::INode> node;
            int thumbnailSize = 0;
        };

        void NodeGraphThumbnail::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::render::NodeGraphThumbnail", parent);
            FTK_P();

            p.node = node;
        }

        NodeGraphThumbnail::NodeGraphThumbnail() :
            _p(new Private)
        {}

        NodeGraphThumbnail::~NodeGraphThumbnail()
        {}

        std::shared_ptr<NodeGraphThumbnail> NodeGraphThumbnail::create(
            const std::shared_ptr<ftk::Context>&context,
            const std::shared_ptr<render::INode>&node,
            const std::shared_ptr<ftk::IWidget>&parent)
        {
            std::shared_ptr<NodeGraphThumbnail> out(new NodeGraphThumbnail);
            out->_init(context, node, parent);
            return out;
        }

        ftk::Size2I NodeGraphThumbnail::getSizeHint() const
        {
            FTK_P();
            return ftk::Size2I(p.thumbnailSize, p.thumbnailSize);
        }

        void NodeGraphThumbnail::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
        }

        void NodeGraphThumbnail::sizeHintEvent(const ftk::SizeHintEvent& event)
        {
            FTK_P();
            p.thumbnailSize = 60 * event.displayScale;
        }
        
        void NodeGraphThumbnail::drawEvent(const ftk::Box2I& drawRect, const ftk::DrawEvent& event)
        {
            FTK_P();
            const ftk::Box2I& g = getGeometry();
            event.render->drawRect(g, ftk::Color4F(0.F, 0.F, 0.F));
            if (!p.node->getOutputs().empty() &&
                p.node->getOutputs().front())
            {
                const auto& output = p.node->getOutputs().front();
                const float aspect = ftk::aspectRatio(output->getSize());
                const ftk::Box2I g2 = render::getBox(aspect, g);
                event.render->drawTexture(output->getColorID(), g2, true);
            }
        }

        struct NodeGraphPopup::Private
        {
        };

        void NodeGraphPopup::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<NodeWidgetFactory>& widgetFactory,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidgetPopup::_init(context, "ibis::render::NodeGraphPopup", parent);
            setWidget(widgetFactory->createWidget(document, node));
        }

        NodeGraphPopup::NodeGraphPopup() :
            _p(new Private)
        {}

        NodeGraphPopup::~NodeGraphPopup()
        {}

        std::shared_ptr<NodeGraphPopup> NodeGraphPopup::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<NodeWidgetFactory>& widgetFactory,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<NodeGraphPopup> out(new NodeGraphPopup);
            out->_init(context, node, document, widgetFactory, parent);
            return out;
        }

        struct NodeGraphWidget::Private
        {
            std::shared_ptr<render::INode> node;
            std::shared_ptr<models::Document> document;
            std::shared_ptr<NodeWidgetFactory> widgetFactory;
            bool selected = false;
            bool view = false;

            std::vector<std::shared_ptr<NodeGraphInput> > inputs;
            std::vector<std::shared_ptr<NodeGraphOutput> > outputs;
            std::shared_ptr<ftk::ToolButton> editButton;
            std::shared_ptr<ftk::ToolButton> viewButton;
            std::shared_ptr<NodeGraphThumbnail> thumbnail;
            std::shared_ptr<ftk::HorizontalLayout> layout;
            std::map<std::string, std::shared_ptr<ftk::Action> > actions;
            std::shared_ptr<NodeGraphPopup> popup;

            std::shared_ptr<ftk::Menu> menu;

            int borderSize = 0;
            int keyFocusSize = 0;

            std::function<void(const std::shared_ptr<render::INode>&)> viewCallback;
        };

        void NodeGraphWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<NodeWidgetFactory>& widgetFactory,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::NodeGraphWidget", parent);
            FTK_P();

            p.node = node;
            p.document = document;
            p.widgetFactory = widgetFactory;

            for (const auto& i : node->getInputs())
            {
                p.inputs.push_back(NodeGraphInput::create(context, node));
            }

            for (const auto& i : node->getOutputs())
            {
                p.outputs.push_back(NodeGraphOutput::create(context, node));
            }

            auto label = ftk::Label::create(
                context,
                ftk::elide(node->getNodeInfo().name, 10));
            label->setHAlign(ftk::HAlign::Center);
            label->setMarginRole(ftk::SizeRole::MarginSmall);
            label->setTooltip(node->getNodeInfo().name);

            p.editButton = ftk::ToolButton::create(context);
            p.editButton->setIcon("Edit");
            p.editButton->setTooltip("Edit the node attributes.");

            p.viewButton = ftk::ToolButton::create(context);
            p.viewButton->setCheckable(true);
            p.viewButton->setIcon("View");
            p.viewButton->setTooltip("Set the view node.");

            p.thumbnail = NodeGraphThumbnail::create(context, node);

            p.actions["Edit"] = ftk::Action::create(
                "Edit",
                "Edit",
                [this]
                {
                    _showPopup();
                });
            p.actions["SetView"] = ftk::Action::create(
                "Set View",
                "View",
                [this](bool value)
                {
                    FTK_P();
                    if (p.viewCallback)
                    {
                        p.viewCallback(value ? p.node : nullptr);
                    }
                });

            p.layout = ftk::HorizontalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(ftk::SizeRole::None);

            auto vLayout = ftk::VerticalLayout::create(context, p.layout);
            vLayout->setSpacingRole(ftk::SizeRole::None);
            for (const auto& i : p.inputs)
            {
                i->setParent(vLayout);
            }

            vLayout = ftk::VerticalLayout::create(context, p.layout);
            vLayout->setSpacingRole(ftk::SizeRole::None);
            auto hLayout = ftk::HorizontalLayout::create(context, vLayout);
            hLayout->setSpacingRole(ftk::SizeRole::None);
            label->setParent(hLayout);
            p.editButton->setParent(hLayout);
            p.viewButton->setParent(hLayout);
            p.thumbnail->setParent(vLayout);

            vLayout = ftk::VerticalLayout::create(context, p.layout);
            vLayout->setSpacingRole(ftk::SizeRole::None);
            for (const auto& i : p.outputs)
            {
                i->setParent(vLayout);
            }

            p.editButton->setClickedCallback(
                [this]
                {
                    _showPopup();
                });

            p.viewButton->setCheckedCallback(
                [this](bool value)
                {
                    FTK_P();
                    if (p.viewCallback)
                    {
                        p.viewCallback(value ? p.node : nullptr);
                    }
                });
        }

        NodeGraphWidget::NodeGraphWidget() :
            _p(new Private)
        {}

        NodeGraphWidget::~NodeGraphWidget()
        {
            FTK_P();
            if (p.popup)
            {
                p.popup->setCloseCallback(nullptr);
                p.popup->close();
            }
        }

        std::shared_ptr<NodeGraphWidget> NodeGraphWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<NodeWidgetFactory>& widgetFactory,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<NodeGraphWidget> out(new NodeGraphWidget);
            out->_init(context, node, document, widgetFactory, parent);
            return out;
        }

        const std::shared_ptr<render::INode>& NodeGraphWidget::getNode() const
        {
            return _p->node;
        }

        const std::vector<std::shared_ptr<NodeGraphInput> >& NodeGraphWidget::getInputs() const
        {
            return _p->inputs;
        }

        const std::vector<std::shared_ptr<NodeGraphOutput> >& NodeGraphWidget::getOutputs() const
        {
            return _p->outputs;
        }

        void NodeGraphWidget::setSelected(bool value)
        {
            FTK_P();
            if (value == p.selected)
                return;
            p.selected = value;
            setDrawUpdate();
        }

        void NodeGraphWidget::setView(bool value)
        {
            FTK_P();
            p.view = value;
            p.viewButton->setChecked(value);
            p.actions["SetView"]->setChecked(value);
        }

        void NodeGraphWidget::setViewCallback(const std::function<void(const std::shared_ptr<render::INode>&)>& value)
        {
            _p->viewCallback = value;
        }

        ftk::Size2I NodeGraphWidget::getSizeHint() const
        {
            FTK_P();
            return p.layout->getSizeHint() + p.keyFocusSize * 2;
        }

        void NodeGraphWidget::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            FTK_P();
            p.layout->setGeometry(ftk::margin(value, -p.keyFocusSize));
        }

        void NodeGraphWidget::sizeHintEvent(const ftk::SizeHintEvent& event)
        {
            FTK_P();
            p.borderSize = event.style->getSizeRole(ftk::SizeRole::Border, event.displayScale);
            p.keyFocusSize = event.style->getSizeRole(ftk::SizeRole::KeyFocus, event.displayScale);
        }

        void NodeGraphWidget::drawEvent(const ftk::Box2I& drawRect, const ftk::DrawEvent& event)
        {
            FTK_P();

            // Draw the border.
            const ftk::Box2I& g = getGeometry();
            event.render->drawMesh(
                ftk::border(g, p.selected ? p.keyFocusSize : p.borderSize),
                event.style->getColorRole(p.selected ? ftk::ColorRole::Checked : ftk::ColorRole::Border));

            // Draw the background.
            const ftk::Box2I g2 = ftk::margin(g, -(p.selected ? p.keyFocusSize : p.borderSize));
            event.render->drawRect(
                g2,
                event.style->getColorRole(ftk::ColorRole::Button));
        }

        void NodeGraphWidget::mousePressEvent(ftk::MouseClickEvent& event)
        {
            FTK_P();
            if ((ftk::MouseButton::Right == event.button && 0 == event.modifiers) ||
                (ftk::MouseButton::Left == event.button && ftk::checkKeyModifier(ftk::KeyModifier::Alt, event.modifiers)))
            {
                event.accept = true;
                p.menu = ftk::Menu::create(getContext());
                p.menu->addAction(p.actions["Edit"]);
                p.menu->addAction(p.actions["SetView"]);

                std::weak_ptr<NodeGraphWidget> weak(std::dynamic_pointer_cast<NodeGraphWidget>(shared_from_this()));
                p.menu->setCloseCallback(
                    [weak]
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->menu.reset();
                        }
                    });
                p.menu->open(
                    getWindow(),
                    ftk::Box2I(event.pos.x, event.pos.y, 0, 0));
            }
        }

        void NodeGraphWidget::mouseReleaseEvent(ftk::MouseClickEvent& event)
        {
            event.accept = true;
        }

        void NodeGraphWidget::_showPopup()
        {
            FTK_P();
            if (!p.popup)
            {
                auto context = getContext();
                p.popup = NodeGraphPopup::create(
                    context,
                    p.node,
                    p.document,
                    p.widgetFactory);
                p.popup->open(getWindow(), p.editButton->getGeometry(), getGeometry());
                p.popup->setCloseCallback(
                    [this]
                    {
                        _p->popup.reset();
                    });
            }
            else
            {
                p.popup->close();
                p.popup.reset();
            }
        }
    }
}
