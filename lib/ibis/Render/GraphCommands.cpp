// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "GraphCommands.h"

namespace ibis
{
    namespace render
    {
        struct AddNodeCommand::Private
        {
            std::shared_ptr<Graph> graph;
            std::shared_ptr<INode> node;
            ftk::V2I pos;
        };

        void AddNodeCommand::_init(
            const std::shared_ptr<Graph>& graph,
            const std::shared_ptr<INode>& node,
            const ftk::V2I& pos)
        {
            FTK_P();
            p.graph = graph;
            p.node = node;
            p.pos = pos;
        }

        AddNodeCommand::AddNodeCommand() :
            _p(new Private)
        {}

        AddNodeCommand::~AddNodeCommand()
        {}

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
            FTK_P();
            p.graph->addNode(p.node, p.pos);
        }

        void AddNodeCommand::undo()
        {
            FTK_P();
            p.graph->removeNode(p.node);
        }
    }
}
