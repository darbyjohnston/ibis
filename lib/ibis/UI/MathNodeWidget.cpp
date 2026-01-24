// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "MathNodeWidget.h"

#include <ibis/Render/MathNode.h>

#include <ftk/UI/Divider.h>
#include <ftk/UI/DoubleEditSlider.h>
#include <ftk/UI/Label.h>
#include <ftk/UI/FormLayout.h>
#include <ftk/UI/RowLayout.h>

namespace ibis
{
    namespace ui
    {
        struct AddValueNodeWidget::Private
        {
            std::shared_ptr<ftk::Label> label;
            std::shared_ptr<ftk::DoubleEditSlider> valueSlider;
            std::shared_ptr<ftk::VerticalLayout> layout;
        };

        void AddValueNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ibis::render::Graph>& graph,
            const std::shared_ptr<ibis::render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            INodeWidget::_init(context, graph, node, parent);
            FTK_P();

            p.label = ftk::Label::create(context, getID());
            p.label->setMarginRole(ftk::SizeRole::MarginSmall);
            p.label->setBackgroundRole(ftk::ColorRole::Button);

            p.valueSlider = ftk::DoubleEditSlider::create(context);

            p.layout = ftk::VerticalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(ftk::SizeRole::None);
            p.label->setParent(p.layout);
            ftk::Divider::create(context, ftk::Orientation::Vertical, p.layout);
            auto formLayout = ftk::FormLayout::create(context, p.layout);
            formLayout->setMarginRole(ftk::SizeRole::Margin);
            formLayout->addRow("Value:", p.valueSlider);
        }

        AddValueNodeWidget::AddValueNodeWidget() :
            _p(new Private)
        {}

        AddValueNodeWidget::~AddValueNodeWidget()
        {}

        std::shared_ptr<AddValueNodeWidget> AddValueNodeWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ibis::render::Graph>& graph,
            const std::shared_ptr<ibis::render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<AddValueNodeWidget> out(new AddValueNodeWidget);
            out->_init(context, graph, node, parent);
            return out;
        }

        std::string AddValueNodeWidget::getNodeID()
        {
            return render::AddValueNode::getNodeID();
        }

        ftk::Size2I AddValueNodeWidget::getSizeHint() const
        {
            return _p->layout->getSizeHint();
        }

        void AddValueNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            INodeWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }
    }
}
