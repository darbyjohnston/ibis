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
        //! Add nodes.
        class AddNodesCmd : public ftk::ICommand
        {
        protected:
            void _init(
                const std::shared_ptr<Graph>&,
                const std::vector<std::shared_ptr<INode> >&,
                const std::vector<ftk::V2I>&);

            AddNodesCmd() = default;

        public:
            virtual ~AddNodesCmd() = default;

            //! Create a new command.
            static std::shared_ptr<AddNodesCmd> create(
                const std::shared_ptr<Graph>&,
                const std::vector<std::shared_ptr<INode> >&,
                const std::vector<ftk::V2I>& = {});

            void exec() override;
            void undo() override;

        private:
            std::shared_ptr<Graph> _graph;
            std::vector<std::shared_ptr<INode> > _nodes;
            std::vector<ftk::V2I> _pos;
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
            std::vector<ftk::V2I> _posPrev;
            std::vector<GraphConnect> _connectPrev;
        };

        //! Move nodes.
        class MoveNodesCmd : public ftk::ICommand
        {
        protected:
            void _init(
                const std::shared_ptr<Graph>&,
                const std::vector<std::shared_ptr<INode> >&,
                const std::vector<ftk::V2I>&);

            MoveNodesCmd() = default;

        public:
            virtual ~MoveNodesCmd() = default;

            //! Create a new command.
            static std::shared_ptr<MoveNodesCmd> create(
                const std::shared_ptr<Graph>&,
                const std::vector<std::shared_ptr<INode> >&,
                const std::vector<ftk::V2I>&);

            void exec() override;
            void undo() override;

        private:
            std::shared_ptr<Graph> _graph;
            std::vector<std::shared_ptr<INode> > _nodes;
            std::vector<ftk::V2I > _pos;
            std::vector<ftk::V2I > _posPrev;
        };

        //! Connect nodes.
        class ConnectNodesCmd : public ftk::ICommand
        {
        protected:
            void _init(
                const std::shared_ptr<Graph>&,
                const GraphConnect&);

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
            GraphConnect _connect;
            NodeConnection _prev;
        };
    }
}
