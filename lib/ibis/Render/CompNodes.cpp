// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "CompNodes.h"

namespace ibis
{
    namespace render
    {
        struct OverNode::Private
        {
        };

        void OverNode::_init(const std::shared_ptr<ftk::Context>& context)
        {
            INode::_init(context, getNodeID(), 2);
            FTK_P();
        }

        OverNode::OverNode() :
            _p(new Private)
        {
        }

        OverNode::~OverNode()
        {
        }

        std::string OverNode::getNodeID()
        {
            return "Over";
        }

        std::shared_ptr<INode> OverNode::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            std::shared_ptr<OverNode> out(new OverNode);
            out->_init(context);
            return out;
        }
    }
}
