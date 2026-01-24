// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ftk/Core/Context.h>
#include <ftk/Core/Observable.h>

#include <opentimelineio/version.h>

#include <nlohmann/json.hpp>

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
            int                    index = -1;

            bool operator == (const NodeConnection&) const;
            bool operator != (const NodeConnection&) const;
        };

        //! Node attributes.
        typedef std::map<std::string, nlohmann::json> NodeAttr;

        //! Node create function.
        typedef std::function<std::shared_ptr<INode>(const std::shared_ptr<ftk::Context>&)> NodeCreate;

        //! Base class for nodes.
        class INode : public std::enable_shared_from_this<INode>
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::string& id,
                int inputCount,
                int outputCount = 1,
                const NodeAttr& = {});

            INode();

        public:
            virtual ~INode() = 0;

            //! Get the ID.
            const std::string& getID() const;

            //! Get the inputs.
            const std::vector<NodeConnection>& getInputs() const;

            //! Set an input.
            void setInput(int, const NodeConnection&);

            //! Get the output count.
            int getOutputCount() const;

            //! Get the attribute keys.
            std::vector<std::string> getAttrKeys() const;

            //! Get an attribute.
            nlohmann::json getAttr(const std::string&) const;

            //! Set an attribute.
            virtual bool setAttr(const std::string&, const nlohmann::json&);

        private:
            FTK_PRIVATE();
        };
    }
}
