// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "Graph.h"

#include "NodeFactory.h"

#include <set>

namespace ibis
{
    namespace render
    {
        struct Graph::Private
        {
            std::shared_ptr<ftk::ObservableList<std::shared_ptr<INode> > > nodes;
            std::map<std::shared_ptr<INode>, ftk::V2I> pos;
            std::shared_ptr<ftk::Observable<bool> > changed;
        };

        namespace
        {
            struct GraphConnectionTmp
            {
                std::shared_ptr<INode> inputNode;
                int input = -1;
                int outputNode = -1;
                int output = -1;
            };
        }

        void Graph::_init(
            const std::shared_ptr<ftk::Context>& context,
            const nlohmann::json& json,
            const std::shared_ptr<render::NodeFactory>& nodeFactory)
        {
            FTK_P();

            std::vector<std::shared_ptr<INode> > nodes;
            if (!json.empty() && nodeFactory)
            {
                if (json.contains("Nodes"))
                {
                    std::vector<GraphConnectionTmp> connections;
                    for (const auto& i : json["Nodes"])
                    {
                        if (i.contains("ID"))
                        {
                            if (auto node = nodeFactory->createNode(
                                i["ID"],
                                i.contains("Attr") ? i["Attr"] : nlohmann::json()))
                            {
                                ftk::V2I pos;
                                if (i.contains("Pos"))
                                {
                                    pos = i["Pos"];
                                }
                                if (i.contains("Inputs"))
                                {
                                    const auto& inputs = i["Inputs"];
                                    for (int j = 0; j < inputs.size(); ++j)
                                    {
                                        connections.push_back({
                                            node,
                                            j,
                                            int(inputs[j]["Index"]),
                                            int(inputs[j]["Output"]) });
                                    }
                                }
                                nodes.push_back(node);
                                p.pos[node] = pos;
                            }
                        }
                    }
                    for (const auto& connection : connections)
                    {
                        if (connection.outputNode >= 0 && connection.outputNode < nodes.size())
                        {
                            connection.inputNode->setInput(
                                connection.input,
                                NodeConnection(nodes[connection.outputNode], connection.output));
                        }
                    }
                }
            }

            p.nodes = ftk::ObservableList<std::shared_ptr<INode> >::create(nodes);
            p.changed = ftk::Observable<bool>::create(false);
        }

        Graph::Graph() :
            _p(new Private)
        {}

        Graph::~Graph()
        {}

        std::shared_ptr<Graph> Graph::create(
            const std::shared_ptr<ftk::Context>& context,
            const nlohmann::json& json,
            const std::shared_ptr<render::NodeFactory>& nodeFactory)
        {
            std::shared_ptr<Graph> out(new Graph);
            out->_init(context, json, nodeFactory);
            return out;
        }

        nlohmann::json Graph::to_json()
        {
            FTK_P();
            nlohmann::json out;
            nlohmann::json nodesJSON;
            const auto& nodes = p.nodes->get();
            for (size_t i = 0; i < nodes.size(); ++i)
            {
                const auto& node = nodes[i];
                nlohmann::json nodeJSON;
                nodeJSON["ID"] = node->getNodeInfo().id;
                nodeJSON["Pos"] = getPos(node);
                const auto& attrKeys = node->getAttrKeys();
                if (!attrKeys.empty())
                {
                    nlohmann::json attrJSON;
                    for (const auto& key : node->getAttrKeys())
                    {
                        attrJSON[key] = node->getAttr(key);
                    }
                    nodeJSON["Attr"] = attrJSON;
                }
                const auto& inputs = node->getInputs();
                if (!inputs.empty())
                {
                    nlohmann::json inputsJSON;
                    for (const auto& input : inputs)
                    {
                        nlohmann::json inputJSON;
                        int index = -1;
                        const auto j = std::find(nodes.begin(), nodes.end(), input.node);
                        if (j != nodes.end())
                        {
                            index = j - nodes.begin();
                        }
                        inputJSON["Index"] = index;
                        inputJSON["Output"] = input.index;
                        inputsJSON.push_back(inputJSON);
                    }
                    nodeJSON["Inputs"] = inputsJSON;
                }
                nodesJSON.push_back(nodeJSON);
            }
            out["Nodes"] = nodesJSON;
            return out;
        }

        void Graph::add(const std::shared_ptr<INode>& node, const ftk::V2I& pos)
        {
            add(
                std::vector<std::shared_ptr<INode> >({ node }),
                std::vector<ftk::V2I>({ pos }));
        }

        void Graph::add(
            const std::vector<std::shared_ptr<INode> >& nodes,
            const std::vector<ftk::V2I>& pos)
        {
            FTK_P();
            auto tmp = p.nodes->get();
            for (size_t i = 0; i < nodes.size(); ++i)
            {
                tmp.push_back(nodes[i]);
                if (i < pos.size())
                {
                    p.pos[nodes[i]] = pos[i];
                }
            }
            p.nodes->setAlways(tmp);
            p.changed->setAlways(true);
        }

        void Graph::remove(const std::shared_ptr<INode>& node)
        {
            remove(std::vector<std::shared_ptr<INode> >({ node }));
        }

        void Graph::remove(const std::vector<std::shared_ptr<INode> >& nodes)
        {
            FTK_P();
            bool changed = false;
            auto tmp = p.nodes->get();
            for (const auto& node : nodes)
            {
                const auto i = std::find(tmp.begin(), tmp.end(), node);
                if (i != tmp.end())
                {
                    // Remove the node.
                    tmp.erase(i);

                    // Remove node connections.
                    for (int j = 0; j < node->getInputs().size(); ++j)
                    {
                        node->setInput(j, NodeConnection());
                    }

                    // Remove connections to the node.
                    for (const auto& node2 : tmp)
                    {
                        const auto& inputs = node2->getInputs();
                        for (int i = 0; i < inputs.size(); ++i)
                        {
                            if (inputs[i].node == node)
                            {
                                node2->setInput(i, NodeConnection());
                            }
                        }
                    }

                    changed = true;
                }

                // Remove the position.
                const auto j = p.pos.find(node);
                if (j != p.pos.end())
                {
                    p.pos.erase(j);
                    changed = true;
                }
            }
            p.nodes->setIfChanged(tmp);
            if (changed)
            {
                p.changed->setAlways(true);
            }
        }

        const std::vector<std::shared_ptr<INode> >& Graph::getNodes() const
        {
            return _p->nodes->get();
        }

        std::shared_ptr<ftk::IObservableList<std::shared_ptr<INode> > > Graph::observeNodes() const
        {
            return _p->nodes;
        }

        void Graph::move(const std::shared_ptr<INode>& node, const ftk::V2I& pos)
        {
            move(
                std::vector<std::shared_ptr<INode> >({ node }),
                std::vector<ftk::V2I>({ pos }));
        }

        void Graph::move(
            const std::vector<std::shared_ptr<INode> >& nodes,
            const std::vector<ftk::V2I>& pos)
        {
            FTK_P();
            bool changed = false;
            for (size_t i = 0; i < nodes.size() && i < pos.size(); ++i)
            {
                const auto j = p.pos.find(nodes[i]);
                if (j != p.pos.end())
                {
                    if (pos[i] != j->second)
                    {
                        j->second = pos[i];
                        changed = true;
                    }
                }
                else
                {
                    p.pos[nodes[i]] = pos[i];
                    changed = true;
                }
            }
            if (changed)
            {
                p.changed->setAlways(true);
            }
        }

        ftk::V2I Graph::getPos(const std::shared_ptr<INode>& node) const
        {
            FTK_P();
            ftk::V2I out;
            const auto i = p.pos.find(node);
            if (i != p.pos.end())
            {
                out = i->second;
            }
            return out;
        }

        std::vector<ftk::V2I> Graph::getPos(
            const std::vector<std::shared_ptr<INode> >& nodes) const
        {
            FTK_P();
            std::vector<ftk::V2I> out;
            for (const auto& node : nodes)
            {
                const auto i = p.pos.find(node);
                if (i != p.pos.end())
                {
                    out.push_back(i->second);
                }
            }
            return out;
        }

        void Graph::setAttr(
            const std::shared_ptr<INode>& node,
            const NodeAttr& attr)
        {
            FTK_P();
            auto tmp = node->getAttr();
            for (const auto& i : attr)
            {
                tmp[i.first] = i.second;
            }
            if (node->setAttr(tmp))
            {
                p.changed->setAlways(true);
            }
        }

        void Graph::setAttr(
            const std::shared_ptr<INode>& node,
            const std::string& key,
            const nlohmann::json& value)
        {
            FTK_P();
            if (node->setAttr(key, value))
            {
                p.changed->setAlways(true);
            }
        }

        std::vector<std::shared_ptr<INode> > Graph::getRootNodes() const
        {
            FTK_P();
            std::vector<std::shared_ptr<INode> > out;
            for (const auto& node : p.nodes->get())
            {
                int inputs = 0;
                for (const auto& input : node->getInputs())
                {
                    if (input.node)
                    {
                        ++inputs;
                    }
                }
                if (!inputs)
                {
                    out.push_back(node);
                }
            }
            return out;
        }

        std::vector<std::shared_ptr<INode> > Graph::getLeafNodes() const
        {
            FTK_P();
            std::set<std::shared_ptr<INode> > outputs;
            for (const auto& node : p.nodes->get())
            {
                for (const auto& input : node->getInputs())
                {
                    if (input.node)
                    {
                        outputs.insert(input.node);
                    }
                }
            }
            std::vector<std::shared_ptr<INode> > out;
            for (const auto& node : p.nodes->get())
            {
                const auto i = outputs.find(node);
                if (i == outputs.end())
                {
                    out.push_back(node);
                }
            }
            return out;
        }

        void Graph::connect(
            const std::shared_ptr<INode>& inputNode,
            int input,
            const std::shared_ptr<INode>& outputNode,
            int output)
        {
            connect({ { inputNode, input, outputNode, output } });
        }

        void Graph::connect(const std::vector<GraphConnect>& connections)
        {
            FTK_P();
            for (const auto& connection : connections)
            {
                connection.inputNode->setInput(
                    connection.input,
                    NodeConnection(connection.outputNode, connection.output));
            }
            p.changed->setAlways(true);
        }

        void Graph::disconnect(
            const std::shared_ptr<INode>& inputNode,
            int input)
        {
            disconnect({ { inputNode, input } });
        }

        void Graph::disconnect(const std::vector<GraphDisconnect>& disconnections)
        {
            FTK_P();
            for (const auto& disconnection : disconnections)
            {
                disconnection.inputNode->setInput(disconnection.input, NodeConnection());
            }
            p.changed->setAlways(true);
        }

        std::shared_ptr<ftk::IObservable<bool> > Graph::observe() const
        {
            return _p->changed;
        }
    }
}