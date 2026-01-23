// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ibis/Render/INode.h>

#include <ftk/Core/Vector.h>

namespace ibis
{
    namespace render
    {
        //! Graph.
        class Graph : public std::enable_shared_from_this<Graph>
        {
        protected:
            void _init(const std::shared_ptr<ftk::Context>&);

            Graph();

        public:
            virtual ~Graph();

            //! Create a new graph.
            static std::shared_ptr<Graph> create(
                const std::shared_ptr<ftk::Context>&);

            //! Add a node to the graph.
            void addNode(const std::shared_ptr<INode>&, const ftk::V2I&);

            //! Remove a node from the graph.
            void removeNode(const std::shared_ptr<INode>&);

            //! Get the nodes in the graph.
            const std::list<std::shared_ptr<INode> >& getNodes() const;

            //! Move a node.
            void moveNode(const std::shared_ptr<INode>&, const ftk::V2I&);

            //! Get a node position.
            ftk::V2I getPos(const std::shared_ptr<INode>&);

            //! Observe graph changes.
            std::shared_ptr<ftk::IObservable<bool> > observe() const;

        private:
            FTK_PRIVATE();
        };
    }
}
