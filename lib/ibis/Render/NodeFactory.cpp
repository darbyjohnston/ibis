// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "NodeFactory.h"

#include "ColorNode.h"
#include "CompNode.h"
#include "GenNode.h"
#include "InputNode.h"
#include "MathNode.h"
#include "TransformNode.h"

#include <set>

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

            p.nodes[ImageFileNode::getClassNodeInfo().id] = &ImageFileNode::create;
            p.info[ImageFileNode::getClassNodeInfo().id] = ImageFileNode::getClassNodeInfo();
            p.nodes[ImageSequenceNode::getClassNodeInfo().id] = &ImageSequenceNode::create;
            p.info[ImageSequenceNode::getClassNodeInfo().id] = ImageSequenceNode::getClassNodeInfo();
            p.nodes[SVGFileNode::getClassNodeInfo().id] = &SVGFileNode::create;
            p.info[SVGFileNode::getClassNodeInfo().id] = SVGFileNode::getClassNodeInfo();

            p.nodes[GradientNode::getClassNodeInfo().id] = &GradientNode::create;
            p.info[GradientNode::getClassNodeInfo().id] = GradientNode::getClassNodeInfo();
            p.nodes[NoiseNode::getClassNodeInfo().id] = &NoiseNode::create;
            p.info[NoiseNode::getClassNodeInfo().id] = NoiseNode::getClassNodeInfo();
            p.nodes[SolidColorNode::getClassNodeInfo().id] = &SolidColorNode::create;
            p.info[SolidColorNode::getClassNodeInfo().id] = SolidColorNode::getClassNodeInfo();

            p.nodes[ArithmeticNode::getClassNodeInfo().id] = &ArithmeticNode::create;
            p.info[ArithmeticNode::getClassNodeInfo().id] = ArithmeticNode::getClassNodeInfo();

            p.nodes[BrightnessNode::getClassNodeInfo().id] = &BrightnessNode::create;
            p.info[BrightnessNode::getClassNodeInfo().id] = BrightnessNode::getClassNodeInfo();
            p.nodes[ContrastNode::getClassNodeInfo().id] = &ContrastNode::create;
            p.info[ContrastNode::getClassNodeInfo().id] = ContrastNode::getClassNodeInfo();
            p.nodes[InvertNode::getClassNodeInfo().id] = &InvertNode::create;
            p.info[InvertNode::getClassNodeInfo().id] = InvertNode::getClassNodeInfo();
            p.nodes[LevelsNode::getClassNodeInfo().id] = &LevelsNode::create;
            p.info[LevelsNode::getClassNodeInfo().id] = LevelsNode::getClassNodeInfo();
            p.nodes[SaturationNode::getClassNodeInfo().id] = &SaturationNode::create;
            p.info[SaturationNode::getClassNodeInfo().id] = SaturationNode::getClassNodeInfo();
            p.nodes[SoftClipNode::getClassNodeInfo().id] = &SoftClipNode::create;
            p.info[SoftClipNode::getClassNodeInfo().id] = SoftClipNode::getClassNodeInfo();
            p.nodes[TintNode::getClassNodeInfo().id] = &TintNode::create;
            p.info[TintNode::getClassNodeInfo().id] = TintNode::getClassNodeInfo();

            p.nodes[ResizeNode::getClassNodeInfo().id] = &ResizeNode::create;
            p.info[ResizeNode::getClassNodeInfo().id] = ResizeNode::getClassNodeInfo();

            p.nodes[OverNode::getClassNodeInfo().id] = &OverNode::create;
            p.info[OverNode::getClassNodeInfo().id] = OverNode::getClassNodeInfo();
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

        const std::map<std::string, NodeInfo>& NodeFactory::getInfo() const
        {
            return _p->info;
        }

        NodeInfo NodeFactory::getInfo(const std::string& id) const
        {
            FTK_P();
            const auto i = p.info.find(id);
            return i != p.info.end() ? i->second : NodeInfo();
        }

        std::vector<std::string> NodeFactory::getIDs() const
        {
            FTK_P();
            std::vector<std::string> out;
            for (const auto& i : p.info)
            {
                out.push_back(i.first);
            }
            return out;
        }

        std::vector<std::string> NodeFactory::getIDs(const std::string& group) const
        {
            FTK_P();
            std::vector<std::string> out;
            for (const auto& i : p.info)
            {
                if (group == i.second.group)
                {
                    out.push_back(i.first);
                }
            }
            return out;
        }

        std::vector<std::string> NodeFactory::getGroups() const
        {
            FTK_P();
            std::set<std::string> groups;
            for (const auto& i : p.info)
            {
                groups.insert(i.second.group);
            }
            return std::vector<std::string>(groups.begin(), groups.end());
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
