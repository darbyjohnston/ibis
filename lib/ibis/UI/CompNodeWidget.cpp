// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "CompNodeWidget.h"

#include <ibis/Render/CompNode.h>

#include <ftk/UI/Bellows.h>
#include <ftk/UI/RowLayout.h>

namespace ibis
{
    namespace ui
    {
        struct OverNodeWidget::Private
        {
            std::shared_ptr<ftk::Bellows> bellows;
        };

        void OverNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ibis::models::Document>& document,
            const std::shared_ptr<ibis::render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            INodeWidget::_init(context, document, node, parent);
            FTK_P();

            auto layout = ftk::VerticalLayout::create(context);
            layout->setMarginRole(ftk::SizeRole::Margin);
            p.bellows = ftk::Bellows::create(context, getID(), shared_from_this());
            p.bellows->setOpen(true);
            p.bellows->setWidget(layout);
        }

        OverNodeWidget::OverNodeWidget() :
            _p(new Private)
        {}

        OverNodeWidget::~OverNodeWidget()
        {}

        std::shared_ptr<OverNodeWidget> OverNodeWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ibis::models::Document>& document,
            const std::shared_ptr<ibis::render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<OverNodeWidget> out(new OverNodeWidget);
            out->_init(context, document, node, parent);
            return out;
        }

        std::string OverNodeWidget::getNodeID()
        {
            return render::OverNode::getNodeID();
        }

        ftk::Size2I OverNodeWidget::getSizeHint() const
        {
            return _p->bellows->getSizeHint();
        }

        void OverNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            INodeWidget::setGeometry(value);
            _p->bellows->setGeometry(value);
        }
    }
}
