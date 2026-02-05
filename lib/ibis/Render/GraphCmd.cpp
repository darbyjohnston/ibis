// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "GraphCmd.h"

namespace ibis
{
    namespace render
    {
        void AddNodesCmd::_init(
            const std::shared_ptr<Graph>& graph,
            const std::vector<std::shared_ptr<INode> >& nodes,
            const std::vector<ftk::V2I>& pos)
        {
            _graph = graph;
            _nodes = nodes;
            _pos = pos;
        }

        std::shared_ptr<AddNodesCmd> AddNodesCmd::create(
            const std::shared_ptr<Graph>& graph,
            const std::vector<std::shared_ptr<INode> >& node,
            const std::vector<ftk::V2I >& pos)
        {
            std::shared_ptr<AddNodesCmd> out(new AddNodesCmd);
            out->_init(graph, node, pos);
            return out;
        }

        void AddNodesCmd::exec()
        {
            _graph->add(_nodes, _pos);
        }

        void AddNodesCmd::undo()
        {
            _graph->remove(_nodes);
        }

        void RemoveNodesCmd::_init(
            const std::shared_ptr<Graph>& graph,
            const std::vector<std::shared_ptr<INode> >& nodes)
        {
            _graph = graph;
            _nodes = nodes;
        }

        std::shared_ptr<RemoveNodesCmd> RemoveNodesCmd::create(
            const std::shared_ptr<Graph>& graph,
            const std::vector<std::shared_ptr<INode> >& nodes)
        {
            std::shared_ptr<RemoveNodesCmd> out(new RemoveNodesCmd);
            out->_init(graph, nodes);
            return out;
        }

        void RemoveNodesCmd::exec()
        {
            // Save node positions.
            _posPrev = _graph->getPos(_nodes);

            // Save node connections.
            _connectPrev.clear();
            for (const auto& node : _nodes)
            {
                const auto& inputs = node->getInputs();
                for (int i = 0; i < inputs.size(); ++i)
                {
                    _connectPrev.push_back({ node, i, inputs[i].node, inputs[i].index });
                }
            }

            // Save connections to the nodes.
            for (const auto& node : _graph->getNodes())
            {
                const auto& inputs = node->getInputs();
                for (int i = 0; i < inputs.size(); ++i)
                {
                    const auto j = std::find(_nodes.begin(), _nodes.end(), inputs[i].node);
                    if (j != _nodes.end())
                    {
                        _connectPrev.push_back({ node, i, inputs[i].node, inputs[i].index });
                    }
                }
            }

            _graph->remove(_nodes);
        }

        void RemoveNodesCmd::undo()
        {
            _graph->add(_nodes, _posPrev);
            _graph->connect(_connectPrev);
        }

        void MoveNodesCmd::_init(
            const std::shared_ptr<Graph>& graph,
            const std::vector<std::shared_ptr<INode> >& nodes,
            const std::vector<ftk::V2I >& pos)
        {
            _graph = graph;
            _nodes = nodes;
            _pos = pos;
        }

        std::shared_ptr<MoveNodesCmd> MoveNodesCmd::create(
            const std::shared_ptr<Graph>& graph,
            const std::vector<std::shared_ptr<INode> >& nodes,
            const std::vector<ftk::V2I>& pos)
        {
            std::shared_ptr<MoveNodesCmd> out(new MoveNodesCmd);
            out->_init(graph, nodes, pos);
            return out;
        }

        void MoveNodesCmd::exec()
        {
            _posPrev = _graph->getPos(_nodes);
            _graph->move(_nodes, _pos);
        }

        void MoveNodesCmd::undo()
        {
            _graph->move(_nodes, _posPrev);
        }

        void ConnectNodesCmd::_init(
            const std::shared_ptr<Graph>& graph,
            const GraphConnect& connect)
        {
            _graph = graph;
            _connect = connect;
        }

        std::shared_ptr<ConnectNodesCmd> ConnectNodesCmd::create(
            const std::shared_ptr<Graph>& graph,
            const std::shared_ptr<INode>& inputNode,
            int input,
            const std::shared_ptr<INode>& outputNode,
            int output)
        {
            std::shared_ptr<ConnectNodesCmd> out(new ConnectNodesCmd);
            out->_init(graph, { inputNode, input, outputNode, output });
            return out;
        }

        void ConnectNodesCmd::exec()
        {
            _prev = _connect.inputNode->getInputs()[_connect.input];
            _graph->connect({ _connect });
        }

        void ConnectNodesCmd::undo()
        {
            _graph->disconnect(_connect.inputNode, _connect.input);
            _graph->connect(_connect.inputNode, _connect.input, _prev.node, _prev.index);
        }

        void NodeAttrCmd::_init(
            const std::shared_ptr<Graph>& graph,
            const std::shared_ptr<INode>& node,
            const NodeAttr& attr)
        {
            _graph = graph;
            _node = node;
            _attr = attr;
            _prev = _node->getAttr();
        }

        std::shared_ptr<NodeAttrCmd> NodeAttrCmd::create(
            const std::shared_ptr<Graph>& graph,
            const std::shared_ptr<INode>& node,
            const NodeAttr& attr)
        {
            std::shared_ptr<NodeAttrCmd> out(new NodeAttrCmd);
            auto tmp = node->getAttr();
            for (const auto& i : attr)
            {
                tmp[i.first] = i.second;
            }
            out->_init(graph, node, tmp);
            return out;
        }

        std::shared_ptr<NodeAttrCmd> NodeAttrCmd::create(
            const std::shared_ptr<Graph>& graph,
            const std::shared_ptr<INode>& node,
            const std::string& key,
            const nlohmann::json& value)
        {
            std::shared_ptr<NodeAttrCmd> out(new NodeAttrCmd);
            NodeAttr tmp = node->getAttr();
            tmp[key] = value;
            out->_init(graph, node, tmp);
            return out;
        }

        void NodeAttrCmd::set(const NodeAttr& value)
        {
            _attr = value;
        }

        void NodeAttrCmd::exec()
        {
            _graph->setAttr(_node, _attr);
        }

        void NodeAttrCmd::undo()
        {
            _graph->setAttr(_node, _prev);
        }
    }
}
