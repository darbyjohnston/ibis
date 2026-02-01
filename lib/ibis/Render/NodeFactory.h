// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ibis/Render/INode.h>

namespace ibis
{
    namespace render
    {
        class INode;

        //! Node factory.
        class NodeFactory : public std::enable_shared_from_this<NodeFactory>
        {
        protected:
            void _init(const std::shared_ptr<ftk::Context>&);

            NodeFactory();

        public:
            virtual ~NodeFactory();

            //! Create a new factory.
            static std::shared_ptr<NodeFactory> create(
                const std::shared_ptr<ftk::Context>&);

            //! Add a node.
            void add(const std::string&, const NodeCreate&);

            //! Get node information.
            const std::map<std::string, NodeInfo>& getInfo() const;

            //! Get node information fo the given ID.
            NodeInfo getInfo(const std::string& id) const;

            //! Get node IDs.
            std::vector<std::string> getIDs() const;

            //! Get node IDs for the given group.
            std::vector<std::string> getIDs(const std::string& group) const;

            //! Get node groups.
            std::vector<std::string> getGroups() const;

            //! Create a node.
            std::shared_ptr<INode> createNode(const std::string&);

        private:
            FTK_PRIVATE();
        };
    }
}
