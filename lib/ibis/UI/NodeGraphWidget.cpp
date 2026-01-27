// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "NodeGraphWidget.h"

#include <ibis/Render/INode.h>

#include <ftk/UI/DrawUtil.h>
#include <ftk/UI/Label.h>
#include <ftk/UI/Icon.h>
#include <ftk/UI/Menu.h>
#include <ftk/UI/RowLayout.h>

namespace ibis
{
    namespace ui
    {
        struct NodeGraphInput::Private
        {
            std::shared_ptr<ftk::Icon> icon;
            std::shared_ptr<ftk::Icon> connectIcon;
        };

        void NodeGraphInput::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IMouseWidget::_init(context, "ibis::NodeGraphInput", parent);
            FTK_P();

            _setMouseHoverEnabled(true);

            p.icon = ftk::Icon::create(context, "NodeInput", shared_from_this());

            p.connectIcon = ftk::Icon::create(context, "NodeInputConnect", shared_from_this());
            p.connectIcon->hide();
        }

        NodeGraphInput::NodeGraphInput() :
            _p(new Private)
        {}

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

        void NodeGraphInput::setConnect(bool value)
        {
            FTK_P();
            p.icon->setVisible(!value);
            p.connectIcon->setVisible(value);
        }

        ftk::Size2I NodeGraphInput::getSizeHint() const
        {
            FTK_P();
            return _p->icon->getSizeHint();
        }

        void NodeGraphInput::setGeometry(const ftk::Box2I& value)
        {
            IMouseWidget::setGeometry(value);
            FTK_P();
            p.icon->setGeometry(value);
            p.connectIcon->setGeometry(value);
        }

        void NodeGraphInput::sizeHintEvent(const ftk::SizeHintEvent& event)
        {}

        void NodeGraphInput::mouseEnterEvent(ftk::MouseEnterEvent & event)
        {
            IMouseWidget::mouseEnterEvent(event);
            setDrawUpdate();
        }

        void NodeGraphInput::mouseLeaveEvent()
        {
            IMouseWidget::mouseLeaveEvent();
            setDrawUpdate();
        }
        
        void NodeGraphInput::drawEvent(const ftk::Box2I& drawRect, const ftk::DrawEvent& event)
        {
            FTK_P();
            if (_isMouseInside())
            {
                event.render->drawRect(
                    getGeometry(),
                    event.style->getColorRole(ftk::ColorRole::Hover));
            }
        }

        struct NodeGraphOutput::Private
        {
            std::shared_ptr<ftk::Icon> icon;
            std::shared_ptr<ftk::Icon> connectIcon;
        };

        void NodeGraphOutput::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IMouseWidget::_init(context, "ibis::NodeGraphOutput", parent);
            FTK_P();

            _setMouseHoverEnabled(true);

            p.icon = ftk::Icon::create(context, "NodeOutput", shared_from_this());

            p.connectIcon = ftk::Icon::create(context, "NodeOutputConnect", shared_from_this());
            p.connectIcon->hide();
        }

        NodeGraphOutput::NodeGraphOutput() :
            _p(new Private)
        {}

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

        void NodeGraphOutput::setConnect(bool value)
        {
            FTK_P();
            p.icon->setVisible(!value);
            p.connectIcon->setVisible(value);
        }

        ftk::Size2I NodeGraphOutput::getSizeHint() const
        {
            return _p->icon->getSizeHint();
        }

        void NodeGraphOutput::setGeometry(const ftk::Box2I& value)
        {
            IMouseWidget::setGeometry(value);
            _p->icon->setGeometry(value);
            _p->connectIcon->setGeometry(value);
        }

        void NodeGraphOutput::sizeHintEvent(const ftk::SizeHintEvent& event)
        {}

        void NodeGraphOutput::mouseEnterEvent(ftk::MouseEnterEvent & event)
        {
            IMouseWidget::mouseEnterEvent(event);
            setDrawUpdate();
        }

        void NodeGraphOutput::mouseLeaveEvent()
        {
            IMouseWidget::mouseLeaveEvent();
            setDrawUpdate();
        }

        void NodeGraphOutput::drawEvent(const ftk::Box2I & drawRect, const ftk::DrawEvent & event)
        {
            FTK_P();
            if (_isMouseInside())
            {
                event.render->drawRect(
                    getGeometry(),
                    event.style->getColorRole(ftk::ColorRole::Hover));
            }
        }

        struct NodeGraphWidget::Private
        {
            std::shared_ptr<render::INode> node;
            bool selected = false;

            std::vector<std::shared_ptr<NodeGraphInput> > inputs;
            std::vector<std::shared_ptr<NodeGraphOutput> > outputs;
            std::shared_ptr<ftk::HorizontalLayout> layout;
            std::shared_ptr<ftk::Menu> menu;

            int borderSize = 0;

            std::function<void(const std::shared_ptr<render::INode>&)> viewCallback;
        };

        void NodeGraphWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::NodeGraphWidget", parent);
            FTK_P();

            p.node = node;

            for (const auto& i : node->getInputs())
            {
                p.inputs.push_back(NodeGraphInput::create(context, node));
            }

            for (const auto& i : node->getOutputs())
            {
                p.outputs.push_back(NodeGraphOutput::create(context, node));
            }

            auto label = ftk::Label::create(context, node->getID());
            label->setHAlign(ftk::HAlign::Center);
            label->setMarginRole(ftk::SizeRole::MarginSmall);

            p.layout = ftk::HorizontalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(ftk::SizeRole::None);

            auto vLayout = ftk::VerticalLayout::create(context, p.layout);
            vLayout->setSpacingRole(ftk::SizeRole::SpacingTool);
            for (const auto& i : p.inputs)
            {
                i->setParent(vLayout);
            }
            label->setParent(p.layout);
            vLayout = ftk::VerticalLayout::create(context, p.layout);
            vLayout->setSpacingRole(ftk::SizeRole::SpacingTool);
            for (const auto& i : p.outputs)
            {
                i->setParent(vLayout);
            }
        }

        NodeGraphWidget::NodeGraphWidget() :
            _p(new Private)
        {}

        NodeGraphWidget::~NodeGraphWidget()
        {}

        std::shared_ptr<NodeGraphWidget> NodeGraphWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<NodeGraphWidget> out(new NodeGraphWidget);
            out->_init(context, node, parent);
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

        void NodeGraphWidget::setViewCallback(const std::function<void(const std::shared_ptr<render::INode>&)>& value)
        {
            _p->viewCallback = value;
        }

        ftk::Size2I NodeGraphWidget::getSizeHint() const
        {
            FTK_P();
            return p.layout->getSizeHint() + p.borderSize * 2;
        }

        void NodeGraphWidget::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            FTK_P();
            p.layout->setGeometry(ftk::margin(value, -p.borderSize));
        }

        void NodeGraphWidget::drawEvent(const ftk::Box2I& drawRect, const ftk::DrawEvent& event)
        {
            FTK_P();
            const ftk::Box2I& g = getGeometry();
            event.render->drawMesh(
                ftk::border(g, p.borderSize),
                event.style->getColorRole(ftk::ColorRole::Border));
            event.render->drawRect(
                ftk::margin(g, -p.borderSize),
                event.style->getColorRole(ftk::ColorRole::Button));
        }

        void NodeGraphWidget::mousePressEvent(ftk::MouseClickEvent& event)
        {
            FTK_P();
            if ((ftk::MouseButton::Right == event.button && 0 == event.modifiers) ||
                (ftk::MouseButton::Left == event.button && ftk::checkKeyModifier(ftk::KeyModifier::Super, event.modifiers)))
            {
                event.accept = true;
                p.menu = ftk::Menu::create(getContext());
                auto action = ftk::Action::create(
                    "Set View",
                    [this]
                    {
                        FTK_P();
                        if (p.viewCallback)
                        {
                            p.viewCallback(p.node);
                        }
                    });
                p.menu->addAction(action);

                std::weak_ptr<NodeGraphWidget> weak(std::dynamic_pointer_cast<NodeGraphWidget>(shared_from_this()));
                p.menu->setCloseCallback(
                    [weak]
                    {
                        if (auto item = weak.lock())
                        {
                            item->_p->menu.reset();
                        }
                    });
                p.menu->open(
                    getWindow(),
                    ftk::Box2I(event.pos.x, event.pos.y, 0, 0));
            }
        }

        void NodeGraphWidget::sizeHintEvent(const ftk::SizeHintEvent& event)
        {
            FTK_P();
            p.borderSize = event.style->getSizeRole(ftk::SizeRole::Border, event.displayScale);
        }

        void NodeGraphWidget::mouseReleaseEvent(ftk::MouseClickEvent& event)
        {
            event.accept = true;
        }
    }
}
