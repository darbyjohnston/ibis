// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ftk/Core/Context.h>
#include <ftk/Core/Vector.h>

namespace ibis
{
    namespace render
    {
        class INode;
        class NodeFactory;

        //! Graph.
        class Graph : public std::enable_shared_from_this<Graph>
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const nlohmann::json&,
                const std::shared_ptr<NodeFactory>&);

            Graph();

        public:
            virtual ~Graph();

            //! Create a new graph.
            static std::shared_ptr<Graph> create(
                const std::shared_ptr<ftk::Context>&,
                const nlohmann::json& = {},
                const std::shared_ptr<NodeFactory>& = nullptr);

            //! Serialize to JSON.
            nlohmann::json to_json();

            //! Add a node to the graph.
            void add(const std::shared_ptr<INode>&, const ftk::V2I& = ftk::V2I());

            //! Add nodes to the graph.
            void add(
                const std::vector<std::shared_ptr<INode> >&,
                const std::vector<ftk::V2I>& = {});

            //! Remove a node from the graph.
            void remove(const std::shared_ptr<INode>&);

            //! Remove nodes from the graph.
            void remove(const std::vector<std::shared_ptr<INode> >&);

            //! Get the nodes in the graph.
            const std::vector<std::shared_ptr<INode> >& getNodes() const;

            //! Move a node.
            void move(const std::shared_ptr<INode>&, const ftk::V2I&);

            //! Get a node position.
            ftk::V2I getPos(const std::shared_ptr<INode>&);

            //! Set a node attribute.
            void setAttr(
                const std::shared_ptr<INode>&,
                const std::string&,
                const nlohmann::json&);

            //! Connect nodes.
            void connect(
                const std::shared_ptr<INode>& inputNode,
                int input,
                const std::shared_ptr<INode>& outputNode,
                int output);

            //! Disconnect nodes.
            void disconnect(
                const std::shared_ptr<INode>& inputNode,
                int input,
                const std::shared_ptr<INode>& outputNode,
                int output);

            //! Get leaf nodes.
            std::vector<std::shared_ptr<INode> > getLeafNodes() const;

            //! Observe graph changes.
            std::shared_ptr<ftk::IObservable<bool> > observe() const;

        private:
            FTK_PRIVATE();
        };
    }
}
