// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ibis/Render/INode.h>

namespace ibis
{
    namespace render
    {
        //! Arithmetic node.
        class ArithmeticNode : public INode
        {
        protected:
            void _init(const std::shared_ptr<ftk::Context>&);

            ArithmeticNode();

        public:
            virtual ~ArithmeticNode();

            static NodeInfo getNodeInfo();

            static std::shared_ptr<INode> create(
                const std::shared_ptr<ftk::Context>&);

            void exec(
                const std::shared_ptr<ftk::IRender>&,
                const OTIO_NS::RationalTime&) override;

        private:
            FTK_PRIVATE();
        };
    }
}
