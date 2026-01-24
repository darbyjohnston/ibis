// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ftk/Core/Context.h>

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

            //! Get node IDs.
            std::vector<std::string> getNodeIDs() const;

            //! Create a node.
            std::shared_ptr<INode> createNode(const std::string&);

        private:
            FTK_PRIVATE();
        };
    }
}
