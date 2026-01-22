// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "NodeBrowser.h"

#include <ftk/UI/Label.h>

namespace ibis
{
    namespace ui
    {
        struct NodeBrowser::Private
        {
            std::shared_ptr<ftk::Label> label;
        };

        void NodeBrowser::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::NodeBrowser", parent);
            FTK_P();
            p.label = ftk::Label::create(context, "Node Browser", shared_from_this());
            p.label->setHAlign(ftk::HAlign::Center);
            p.label->setMarginRole(ftk::SizeRole::MarginLarge);
        }

        NodeBrowser::NodeBrowser() :
            _p(new Private)
        {}

        NodeBrowser::~NodeBrowser()
        {}

        std::shared_ptr<NodeBrowser> NodeBrowser::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<NodeBrowser> out(new NodeBrowser);
            out->_init(context, parent);
            return out;
        }

        ftk::Size2I NodeBrowser::getSizeHint() const
        {
            return _p->label->getSizeHint();
        }

        void NodeBrowser::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->label->setGeometry(value);
        }
    }
}
