// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "NodeFactory.h"

#include "ColorNode.h"
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
            std::map<std::string, NodeInfo> info;
        };

        void NodeFactory::_init(const std::shared_ptr<ftk::Context>& context)
        {
            FTK_P();

            p.context = context;

            p.nodes[ArithmeticNode::getNodeInfo().id] = &ArithmeticNode::create;
            p.info[ArithmeticNode::getNodeInfo().id] = ArithmeticNode::getNodeInfo();

            p.nodes[BrightnessNode::getNodeInfo().id] = &BrightnessNode::create;
            p.info[BrightnessNode::getNodeInfo().id] = BrightnessNode::getNodeInfo();

            p.nodes[ContrastNode::getNodeInfo().id] = &ContrastNode::create;
            p.info[ContrastNode::getNodeInfo().id] = ContrastNode::getNodeInfo();

            p.nodes[ImageFileNode::getNodeInfo().id] = &ImageFileNode::create;
            p.info[ImageFileNode::getNodeInfo().id] = ImageFileNode::getNodeInfo();

            p.nodes[ImageSequenceNode::getNodeInfo().id] = &ImageSequenceNode::create;
            p.info[ImageSequenceNode::getNodeInfo().id] = ImageSequenceNode::getNodeInfo();

            p.nodes[OverNode::getNodeInfo().id] = &OverNode::create;
            p.info[OverNode::getNodeInfo().id] = OverNode::getNodeInfo();

            p.nodes[SVGFileNode::getNodeInfo().id] = &SVGFileNode::create;
            p.info[SVGFileNode::getNodeInfo().id] = SVGFileNode::getNodeInfo();

            p.nodes[SolidColorNode::getNodeInfo().id] = &SolidColorNode::create;
            p.info[SolidColorNode::getNodeInfo().id] = SolidColorNode::getNodeInfo();
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

        std::vector<NodeInfo> NodeFactory::getInfo() const
        {
            FTK_P();
            std::vector<NodeInfo> out;
            for (auto i = p.info.begin(); i != p.info.end(); ++i)
            {
                out.push_back(i->second);
            }
            return out;
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
