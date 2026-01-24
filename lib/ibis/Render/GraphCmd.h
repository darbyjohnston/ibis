// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ibis/Render/Graph.h>

#include <ftk/Core/Command.h>

namespace ibis
{
    namespace render
    {
        //! Add a node.
        class AddNodeCommand : public ftk::ICommand
        {
        protected:
            void _init(
                const std::shared_ptr<Graph>&,
                const std::shared_ptr<INode>&,
                const ftk::V2I&);

            AddNodeCommand() = default;

        public:
            virtual ~AddNodeCommand() = default;

            //! Create a new command.
            static std::shared_ptr<AddNodeCommand> create(
                const std::shared_ptr<Graph>&,
                const std::shared_ptr<INode>&,
                const ftk::V2I&);

            void exec() override;
            void undo() override;

        private:
            std::shared_ptr<Graph> _graph;
            std::shared_ptr<INode> _node;
            ftk::V2I _pos;
        };

        //! Move a node.
        class MoveNodeCommand : public ftk::ICommand
        {
        protected:
            void _init(
                const std::shared_ptr<Graph>&,
                const std::shared_ptr<INode>&,
                const ftk::V2I&);

            MoveNodeCommand() = default;

        public:
            virtual ~MoveNodeCommand() = default;

            //! Create a new command.
            static std::shared_ptr<MoveNodeCommand> create(
                const std::shared_ptr<Graph>&,
                const std::shared_ptr<INode>&,
                const ftk::V2I&);

            void exec() override;
            void undo() override;

        private:
            std::shared_ptr<Graph> _graph;
            std::shared_ptr<INode> _node;
            ftk::V2I _pos;
            ftk::V2I _posPrev;
        };

        //! Connect nodes.
        class ConnectNodesCommand : public ftk::ICommand
        {
        protected:
            void _init(
                const std::shared_ptr<Graph>&,
                const std::shared_ptr<INode>&,
                int input,
                const std::shared_ptr<INode>&,
                int output);

            ConnectNodesCommand() = default;

        public:
            virtual ~ConnectNodesCommand() = default;

            //! Create a new command.
            static std::shared_ptr<ConnectNodesCommand> create(
                const std::shared_ptr<Graph>&,
                const std::shared_ptr<INode>& inputNode,
                int input,
                const std::shared_ptr<INode>& outputNode,
                int output);

            void exec() override;
            void undo() override;

        private:
            std::shared_ptr<Graph> _graph;
            std::shared_ptr<INode> _inputNode;
            int _input = -1;
            std::shared_ptr<INode> _outputNode;
            int _output = -1;
            NodeConnection _prev;
        };
    }
}
