// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ibis/Render/Graph.h>
#include <ibis/Render/INode.h>

#include <ftk/Core/Command.h>

namespace ibis
{
    namespace render
    {
        //! Add a node.
        class AddNodeCmd : public ftk::ICommand
        {
        protected:
            void _init(
                const std::shared_ptr<Graph>&,
                const std::shared_ptr<INode>&,
                const ftk::V2I&);

            AddNodeCmd() = default;

        public:
            virtual ~AddNodeCmd() = default;

            //! Create a new command.
            static std::shared_ptr<AddNodeCmd> create(
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

        //! Remove nodes.
        class RemoveNodesCmd : public ftk::ICommand
        {
        protected:
            void _init(
                const std::shared_ptr<Graph>&,
                const std::vector<std::shared_ptr<INode> >&);

            RemoveNodesCmd() = default;

        public:
            virtual ~RemoveNodesCmd() = default;

            //! Create a new command.
            static std::shared_ptr<RemoveNodesCmd> create(
                const std::shared_ptr<Graph>&,
                const std::vector<std::shared_ptr<INode> >&);

            void exec() override;
            void undo() override;

        private:
            std::shared_ptr<Graph> _graph;
            std::vector<std::shared_ptr<INode> > _nodes;
            std::vector<NodeConnection> _connections;
        };

        //! Move a node.
        class MoveNodeCmd : public ftk::ICommand
        {
        protected:
            void _init(
                const std::shared_ptr<Graph>&,
                const std::shared_ptr<INode>&,
                const ftk::V2I&);

            MoveNodeCmd() = default;

        public:
            virtual ~MoveNodeCmd() = default;

            //! Create a new command.
            static std::shared_ptr<MoveNodeCmd> create(
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
        class ConnectNodesCmd : public ftk::ICommand
        {
        protected:
            void _init(
                const std::shared_ptr<Graph>&,
                const std::shared_ptr<INode>&,
                int input,
                const std::shared_ptr<INode>&,
                int output);

            ConnectNodesCmd() = default;

        public:
            virtual ~ConnectNodesCmd() = default;

            //! Create a new command.
            static std::shared_ptr<ConnectNodesCmd> create(
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
