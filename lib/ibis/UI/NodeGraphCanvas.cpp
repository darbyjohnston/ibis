// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "NodeGraphCanvas.h"

#include <ftk/UI/Label.h>

namespace ibis
{
    namespace ui
    {
        struct NodeGraphCanvas::Private
        {
            std::shared_ptr<ftk::Label> label;
        };

        void NodeGraphCanvas::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::NodeGraphCanvas", parent);
            FTK_P();
            p.label = ftk::Label::create(context, "Node Graph Canvas", shared_from_this());
            p.label->setHAlign(ftk::HAlign::Center);
            p.label->setMarginRole(ftk::SizeRole::MarginLarge);
        }

        NodeGraphCanvas::NodeGraphCanvas() :
            _p(new Private)
        {}

        NodeGraphCanvas::~NodeGraphCanvas()
        {}

        std::shared_ptr<NodeGraphCanvas> NodeGraphCanvas::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<NodeGraphCanvas> out(new NodeGraphCanvas);
            out->_init(context, parent);
            return out;
        }

        ftk::Size2I NodeGraphCanvas::getSizeHint() const
        {
            return _p->label->getSizeHint();
        }

        void NodeGraphCanvas::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->label->setGeometry(value);
        }
    }
}
