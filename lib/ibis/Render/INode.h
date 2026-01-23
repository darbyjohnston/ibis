// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ftk/Core/Context.h>
#include <ftk/Core/Observable.h>

#include <opentimelineio/version.h>

namespace ibis
{
    namespace render
    {
        class INode;

        //! Node connection.
        struct NodeConnection
        {
            NodeConnection() = default;
            NodeConnection(const std::shared_ptr<INode>&, int index);

            std::shared_ptr<INode> node;
            int index = -1;

            bool operator == (const NodeConnection&) const;
            bool operator != (const NodeConnection&) const;
        };

        //! Base class for nodes.
        class INode : public std::enable_shared_from_this<INode>
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::string& id,
                int inputCount,
                int outputCount = 1);

            INode();

        public:
            virtual ~INode();

            const std::string& getID() const;

            const std::vector<NodeConnection>& getInputs() const;

            void setInput(int, const NodeConnection&);

            int getOutputCount() const;

        private:
            FTK_PRIVATE();
        };
    }
}
