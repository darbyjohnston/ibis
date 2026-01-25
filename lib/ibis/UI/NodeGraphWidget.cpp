// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "NodeGraphWidget.h"

#include <ibis/Render/INode.h>

#include <ftk/UI/Label.h>
#include <ftk/UI/RowLayout.h>

namespace ibis
{
    namespace ui
    {
        struct NodeGraphInput::Private
        {
            int sizeHint = 0;
        };

        void NodeGraphInput::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::NodeGraphInput", parent);
            FTK_P();

            setBackgroundRole(ftk::ColorRole::Blue);
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

        ftk::Size2I NodeGraphInput::getSizeHint() const
        {
            FTK_P();
            return ftk::Size2I(p.sizeHint, p.sizeHint);
        }

        void NodeGraphInput::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
        }

        void NodeGraphInput::sizeHintEvent(const ftk::SizeHintEvent& event)
        {
            FTK_P();
            p.sizeHint = event.style->getSizeRole(ftk::SizeRole::Handle, event.displayScale) * 1.5;
        }

        struct NodeGraphOutput::Private
        {
            int sizeHint = 0;
        };

        void NodeGraphOutput::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::NodeGraphOutput", parent);
            FTK_P();

            setBackgroundRole(ftk::ColorRole::Green);
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

        ftk::Size2I NodeGraphOutput::getSizeHint() const
        {
            FTK_P();
            return ftk::Size2I(p.sizeHint, p.sizeHint);
        }

        void NodeGraphOutput::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
        }

        void NodeGraphOutput::sizeHintEvent(const ftk::SizeHintEvent& event)
        {
            FTK_P();
            p.sizeHint = event.style->getSizeRole(ftk::SizeRole::Handle, event.displayScale) * 1.5;
        }

        struct NodeGraphWidget::Private
        {
            std::shared_ptr<render::INode> node;
            std::vector<std::shared_ptr<NodeGraphInput> > inputs;
            std::vector<std::shared_ptr<NodeGraphOutput> > outputs;
            bool selected = false;
            std::shared_ptr<ftk::VerticalLayout> layout;
        };

        void NodeGraphWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::NodeGraphWidget", parent);
            FTK_P();

            setBackgroundRole(ftk::ColorRole::Button);

            p.node = node;

            auto label = ftk::Label::create(context, node->getID());
            label->setHAlign(ftk::HAlign::Center);
            label->setMarginRole(ftk::SizeRole::MarginSmall);

            p.layout = ftk::VerticalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(ftk::SizeRole::None);
            auto hLayout = ftk::HorizontalLayout::create(context, p.layout);
            p.layout->setSpacingRole(ftk::SizeRole::SpacingTool);
            for (const auto& i : node->getInputs())
            {
                p.inputs.push_back(NodeGraphInput::create(context, node, hLayout));
            }
            label->setParent(p.layout);
            hLayout = ftk::HorizontalLayout::create(context, p.layout);
            p.layout->setSpacingRole(ftk::SizeRole::SpacingTool);
            for (int i = 0; i < node->getOutputs().size(); ++i)
            {
                p.outputs.push_back(NodeGraphOutput::create(context, node, hLayout));
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

        bool NodeGraphWidget::isSelected() const
        {
            return _p->selected;
        }

        void NodeGraphWidget::setSelected(bool value)
        {
            FTK_P();
            if (value == p.selected)
                return;
            p.selected = value;
            setBackgroundRole(
                p.selected ?
                ftk::ColorRole::Checked :
                ftk::ColorRole::Button);
        }

        ftk::Size2I NodeGraphWidget::getSizeHint() const
        {
            return _p->layout->getSizeHint();
        }

        void NodeGraphWidget::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }
    }
}
