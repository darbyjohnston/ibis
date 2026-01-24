// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "MathNode.h"

namespace ibis
{
    namespace render
    {
        struct AddValueNode::Private
        {
        };

        void AddValueNode::_init(const std::shared_ptr<ftk::Context>& context)
        {
            NodeAttr attr;
            attr["Value"] = 0.0;
            INode::_init(context, getNodeID(), 1, 1, attr);
            FTK_P();
        }

        AddValueNode::AddValueNode() :
            _p(new Private)
        {}

        AddValueNode::~AddValueNode()
        {}

        std::string AddValueNode::getNodeID()
        {
            return "AddValue";
        }

        std::shared_ptr<INode> AddValueNode::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            std::shared_ptr<AddValueNode> out(new AddValueNode);
            out->_init(context);
            return out;
        }
    }
}
