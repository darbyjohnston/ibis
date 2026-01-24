// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "GenNode.h"

#include <ftk/Core/Color.h>

namespace ibis
{
    namespace render
    {
        struct SolidColorNode::Private
        {
        };

        void SolidColorNode::_init(const std::shared_ptr<ftk::Context>& context)
        {
            NodeAttr attr;
            attr["Color"] = ftk::Color4F(1.F, .7F, 0.F);
            INode::_init(context, getNodeID(), 0, 1, attr);
            FTK_P();
        }

        SolidColorNode::SolidColorNode() :
            _p(new Private)
        {}

        SolidColorNode::~SolidColorNode()
        {}

        std::string SolidColorNode::getNodeID()
        {
            return "SolidColor";
        }

        std::shared_ptr<INode> SolidColorNode::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            std::shared_ptr<SolidColorNode> out(new SolidColorNode);
            out->_init(context);
            return out;
        }
    }
}
