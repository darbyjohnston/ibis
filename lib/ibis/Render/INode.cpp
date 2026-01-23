// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "INode.h"

namespace ibis
{
    namespace render
    {
        NodeConnection::NodeConnection(const std::shared_ptr<INode>& node, int index) :
            node(node),
            index(index)
        {}

        bool NodeConnection::operator == (const NodeConnection& other) const
        {
            return node == other.node && index == other.index;
        }

        bool NodeConnection::operator != (const NodeConnection& other) const
        {
            return !(*this == other);
        }

        struct INode::Private
        {
            std::string name;
            std::vector<NodeConnection> inputs;
            int outputCount = -1;
        };

        void INode::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::string& name,
            int inputCount,
            int outputCount)
        {
            FTK_P();
            p.name = name;
            p.inputs.resize(inputCount);
            p.outputCount = outputCount;
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

        const std::vector<NodeConnection>& INode::getInputs() const
        {
            return _p->inputs;
        }

        void INode::setInput(int index, const NodeConnection& connection)
        {
            FTK_P();
            if (index >= 0 && index < p.inputs.size())
            {
                p.inputs[index] = connection;
            }
        }

        int INode::getOutputCount() const
        {
            return _p->outputCount;
        }
    }
}