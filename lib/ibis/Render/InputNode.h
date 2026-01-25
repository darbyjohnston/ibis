// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ibis/Render/INode.h>

namespace ibis
{
    namespace render
    {
        //! Input node.
        class InputNode : public INode
        {
        protected:
            void _init(const std::shared_ptr<ftk::Context>&);

            InputNode();

        public:
            virtual ~InputNode();

            static std::string getNodeID();

            static std::shared_ptr<INode> create(
                const std::shared_ptr<ftk::Context>&);

            void exec(const std::shared_ptr<ftk::IRender>&) override;

        private:
            FTK_PRIVATE();
        };
    }
}
