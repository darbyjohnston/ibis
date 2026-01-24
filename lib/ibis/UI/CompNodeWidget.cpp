// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "CompNodeWidget.h"

#include <ibis/Render/CompNode.h>

#include <ftk/UI/Divider.h>
#include <ftk/UI/Label.h>
#include <ftk/UI/RowLayout.h>

namespace ibis
{
    namespace ui
    {
        struct OverNodeWidget::Private
        {
            std::shared_ptr<ftk::Label> label;
            std::shared_ptr<ftk::VerticalLayout> layout;
        };

        void OverNodeWidget::_init(
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

            p.layout = ftk::VerticalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(ftk::SizeRole::None);
            p.label->setParent(p.layout);
            ftk::Divider::create(context, ftk::Orientation::Vertical, p.layout);
        }

        OverNodeWidget::OverNodeWidget() :
            _p(new Private)
        {}

        OverNodeWidget::~OverNodeWidget()
        {}

        std::shared_ptr<OverNodeWidget> OverNodeWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ibis::render::Graph>& graph,
            const std::shared_ptr<ibis::render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<OverNodeWidget> out(new OverNodeWidget);
            out->_init(context, graph, node, parent);
            return out;
        }

        std::string OverNodeWidget::getNodeID()
        {
            return render::OverNode::getNodeID();
        }

        ftk::Size2I OverNodeWidget::getSizeHint() const
        {
            return _p->layout->getSizeHint();
        }

        void OverNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            INodeWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }
    }
}
