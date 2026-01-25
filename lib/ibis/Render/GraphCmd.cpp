// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "GraphCmd.h"

namespace ibis
{
    namespace render
    {
        void AddNodeCmd::_init(
            const std::shared_ptr<Graph>& graph,
            const std::shared_ptr<INode>& node,
            const ftk::V2I& pos)
        {
            _graph = graph;
            _node = node;
            _pos = pos;
        }

        std::shared_ptr<AddNodeCmd> AddNodeCmd::create(
            const std::shared_ptr<Graph>& graph,
            const std::shared_ptr<INode>& node,
            const ftk::V2I& pos)
        {
            std::shared_ptr<AddNodeCmd> out(new AddNodeCmd);
            out->_init(graph, node, pos);
            return out;
        }

        void AddNodeCmd::exec()
        {
            _graph->add(_node, _pos);
        }

        void AddNodeCmd::undo()
        {
            _graph->remove(_node);
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
            _graph->remove(_nodes);
        }

        void RemoveNodesCmd::undo()
        {
            _graph->add(_nodes);
        }

        void MoveNodeCmd::_init(
            const std::shared_ptr<Graph>& graph,
            const std::shared_ptr<INode>& node,
            const ftk::V2I& pos)
        {
            _graph = graph;
            _node = node;
            _pos = pos;
        }

        std::shared_ptr<MoveNodeCmd> MoveNodeCmd::create(
            const std::shared_ptr<Graph>& graph,
            const std::shared_ptr<INode>& node,
            const ftk::V2I& pos)
        {
            std::shared_ptr<MoveNodeCmd> out(new MoveNodeCmd);
            out->_init(graph, node, pos);
            return out;
        }

        void MoveNodeCmd::exec()
        {
            _posPrev = _graph->getPos(_node);
            _graph->move(_node, _pos);
        }

        void MoveNodeCmd::undo()
        {
            _graph->move(_node, _posPrev);
        }

        void ConnectNodesCmd::_init(
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

        std::shared_ptr<ConnectNodesCmd> ConnectNodesCmd::create(
            const std::shared_ptr<Graph>& graph,
            const std::shared_ptr<INode>& inputNode,
            int input,
            const std::shared_ptr<INode>& outputNode,
            int output)
        {
            std::shared_ptr<ConnectNodesCmd> out(new ConnectNodesCmd);
            out->_init(graph, inputNode, input, outputNode, output);
            return out;
        }

        void ConnectNodesCmd::exec()
        {
            _prev = _inputNode->getInputs()[_input];
            _graph->connect(_inputNode, _input, _outputNode, _output);
        }

        void ConnectNodesCmd::undo()
        {
            _graph->disconnect(_inputNode, _input, _outputNode, _output);
            _graph->connect(_inputNode, _input, _prev.node, _prev.index);
        }
    }
}
