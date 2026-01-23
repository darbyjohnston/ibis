// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ibis/Render/INode.h>

namespace ibis
{
    namespace render
    {
        //! Add value node.
        class AddValueNode : public INode
        {
        protected:
            void _init(const std::shared_ptr<ftk::Context>&);

            AddValueNode();

        public:
            virtual ~AddValueNode();

            static std::string getNodeID();

            static std::shared_ptr<INode> create(
                const std::shared_ptr<ftk::Context>&);

        private:
            FTK_PRIVATE();
        };
    }
}
