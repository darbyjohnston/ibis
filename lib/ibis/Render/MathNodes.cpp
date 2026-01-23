// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "MathNodes.h"

namespace ibis
{
    namespace render
    {
        struct AddValueNode::Private
        {
        };

        void AddValueNode::_init(const std::shared_ptr<ftk::Context>& context)
        {
            INode::_init(context, "AddValue", 1);
            FTK_P();
        }

        AddValueNode::AddValueNode() :
            _p(new Private)
        {}

        AddValueNode::~AddValueNode()
        {}

        std::string AddValueNode::getNodeName()
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
