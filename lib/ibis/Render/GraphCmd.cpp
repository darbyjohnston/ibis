// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "GraphCmd.h"

namespace ibis
{
    namespace render
    {
        void AddNodeCommand::_init(
            const std::shared_ptr<Graph>& graph,
            const std::shared_ptr<INode>& node,
            const ftk::V2I& pos)
        {
            _graph = graph;
            _node = node;
            _pos = pos;
        }

        std::shared_ptr<AddNodeCommand> AddNodeCommand::create(
            const std::shared_ptr<Graph>& graph,
            const std::shared_ptr<INode>& node,
            const ftk::V2I& pos)
        {
            std::shared_ptr<AddNodeCommand> out(new AddNodeCommand);
            out->_init(graph, node, pos);
            return out;
        }

        void AddNodeCommand::exec()
        {
            _graph->add(_node, _pos);
        }

        void AddNodeCommand::undo()
        {
            _graph->remove(_node);
        }

        void RemoveNodesCommand::_init(
            const std::shared_ptr<Graph>& graph,
            const std::vector<std::shared_ptr<INode> >& nodes)
        {
            _graph = graph;
            _nodes = nodes;
        }

        std::shared_ptr<RemoveNodesCommand> RemoveNodesCommand::create(
            const std::shared_ptr<Graph>& graph,
            const std::vector<std::shared_ptr<INode> >& nodes)
        {
            std::shared_ptr<RemoveNodesCommand> out(new RemoveNodesCommand);
            out->_init(graph, nodes);
            return out;
        }

        void RemoveNodesCommand::exec()
        {
            _graph->remove(_nodes);
        }

        void RemoveNodesCommand::undo()
        {
            _graph->add(_nodes);
        }

        void MoveNodeCommand::_init(
            const std::shared_ptr<Graph>& graph,
            const std::shared_ptr<INode>& node,
            const ftk::V2I& pos)
        {
            _graph = graph;
            _node = node;
            _pos = pos;
        }

        std::shared_ptr<MoveNodeCommand> MoveNodeCommand::create(
            const std::shared_ptr<Graph>& graph,
            const std::shared_ptr<INode>& node,
            const ftk::V2I& pos)
        {
            std::shared_ptr<MoveNodeCommand> out(new MoveNodeCommand);
            out->_init(graph, node, pos);
            return out;
        }

        void MoveNodeCommand::exec()
        {
            _posPrev = _graph->getPos(_node);
            _graph->move(_node, _pos);
        }

        void MoveNodeCommand::undo()
        {
            _graph->move(_node, _posPrev);
        }

        void ConnectNodesCommand::_init(
            const std::shared_ptr<Graph>& graph,
            const std::shared_ptr<INode>& inputNode,
            int input,
            const std::shared_ptr<INode>& outputNode,
            int output)
        {
            _graph = graph;
            _inputNode = inputNode;
            _input = input;
            _outputNode = outputNode;
            _output = output;
        }

        std::shared_ptr<ConnectNodesCommand> ConnectNodesCommand::create(
            const std::shared_ptr<Graph>& graph,
            const std::shared_ptr<INode>& inputNode,
            int input,
            const std::shared_ptr<INode>& outputNode,
            int output)
        {
            std::shared_ptr<ConnectNodesCommand> out(new ConnectNodesCommand);
            out->_init(graph, inputNode, input, outputNode, output);
            return out;
        }

        void ConnectNodesCommand::exec()
        {
            _prev = _inputNode->getInputs()[_input];
            _graph->connect(_inputNode, _input, _outputNode, _output);
        }

        void ConnectNodesCommand::undo()
        {
            _graph->disconnect(_inputNode, _input, _outputNode, _output);
            _graph->connect(_inputNode, _input, _prev.node, _prev.index);
        }
    }
}
