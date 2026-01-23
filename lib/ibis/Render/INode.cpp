// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "INode.h"

namespace ibis
{
    namespace render
    {
        struct INode::Private
        {
            std::string name;
        };

        void INode::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::string& name)
        {
            FTK_P();
            p.name = name;
        }

        INode::INode() :
            _p(new Private)
        {}

        INode::~INode()
        {}

        const std::string& INode::getName() const
        {
            return _p->name;
        }
    }
}