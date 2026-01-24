// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "NodeFactory.h"

#include "CompNode.h"
#include "IONode.h"
#include "MathNode.h"

namespace ibis
{
    namespace render
    {
        struct NodeFactory::Private
        {
            std::weak_ptr<ftk::Context> context;

            std::map<
                std::string,
                std::function<std::shared_ptr<INode>(const std::shared_ptr<ftk::Context>&)> >
                nodes;
        };

        void NodeFactory::_init(const std::shared_ptr<ftk::Context>& context)
        {
            FTK_P();

            p.context = context;

            p.nodes[InputNode::getNodeID()] = &InputNode::create;
            p.nodes[OutputNode::getNodeID()] = &OutputNode::create;
            p.nodes[OverNode::getNodeID()] = &OverNode::create;
            p.nodes[AddValueNode::getNodeID()] = &AddValueNode::create;
        }

        NodeFactory::NodeFactory() :
            _p(new Private)
        {}

        NodeFactory::~NodeFactory()
        {}

        std::shared_ptr<NodeFactory> NodeFactory::create(const std::shared_ptr<ftk::Context>& context)
        {
            std::shared_ptr<NodeFactory> out(new NodeFactory);
            out->_init(context);
            return out;
        }

        std::vector<std::string> NodeFactory::getNodeIDs() const
        {
            FTK_P();
            std::vector<std::string> out;
            for (auto i = p.nodes.begin(); i != p.nodes.end(); ++i)
            {
                out.push_back(i->first);
            }
            return out;
        }

        std::shared_ptr<INode> NodeFactory::createNode(const std::string& id)
        {
            FTK_P();
            std::shared_ptr<INode> out;
            const auto i = p.nodes.find(id);
            if (i != p.nodes.end())
            {
                out = i->second(p.context.lock());
            }
            return out;
        }
    }
}
