// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "INodeWidget.h"

#include <ibis/Render/INode.h>

namespace ibis
{
    namespace ui
    {
        void INodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ibis::models::Document>& document,
            const std::shared_ptr<ibis::render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::INodeWidget", parent);
            _document = document;
            _node = node;
        }

        INodeWidget::~INodeWidget()
        {}

        const render::NodeInfo& INodeWidget::getNodeInfo() const
        {
            return _node->getNodeInfo();
        }

        const std::shared_ptr<ibis::render::INode>& INodeWidget::getNode() const
        {
            return _node;
        }
    }
}
