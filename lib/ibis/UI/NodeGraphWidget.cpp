// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "NodeGraphWidget.h"

#include <ftk/UI/Label.h>

namespace ibis
{
    namespace ui
    {
        struct NodeGraphWidget::Private
        {
            std::shared_ptr<ftk::Label> label;
        };

        void NodeGraphWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::NodeGraphWidget", parent);
            FTK_P();
            p.label = ftk::Label::create(context, "Node Graph Widget", shared_from_this());
            p.label->setHAlign(ftk::HAlign::Center);
            p.label->setMarginRole(ftk::SizeRole::MarginLarge);
        }

        NodeGraphWidget::NodeGraphWidget() :
            _p(new Private)
        {}

        NodeGraphWidget::~NodeGraphWidget()
        {}

        std::shared_ptr<NodeGraphWidget> NodeGraphWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<NodeGraphWidget> out(new NodeGraphWidget);
            out->_init(context, parent);
            return out;
        }

        ftk::Size2I NodeGraphWidget::getSizeHint() const
        {
            return _p->label->getSizeHint();
        }

        void NodeGraphWidget::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->label->setGeometry(value);
        }
    }
}
