// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "NodeFactory.h"

#include "CompNode.h"
#include "GenNode.h"
#include "InputNode.h"
#include "MathNode.h"

namespace ibis
{
    namespace render
    {
        struct NodeFactory::Private
        {
            std::weak_ptr<ftk::Context> context;
            std::map<std::string, NodeCreate> nodes;
        };

        void NodeFactory::_init(const std::shared_ptr<ftk::Context>& context)
        {
            FTK_P();

            p.context = context;

            p.nodes[AddValueNode::getNodeID()] = &AddValueNode::create;
            p.nodes[InputNode::getNodeID()] = &InputNode::create;
            p.nodes[OverNode::getNodeID()] = &OverNode::create;
            p.nodes[SolidColorNode::getNodeID()] = &SolidColorNode::create;
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

        void NodeFactory::add(const std::string& id, const NodeCreate& create)
        {
            _p->nodes[id] = create;
        }

        std::vector<std::string> NodeFactory::getIDs() const
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
