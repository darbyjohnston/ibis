// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ibis/Render/INode.h>

namespace ibis
{
    namespace render
    {
        //! Over mode.
        enum class OverMode
        {
            Premult,
            NonPremult,
            Add,
            Subtract,
            Multiply,
            Divide,
            Min,
            Max,

            Count,
            First = Premult
        };
        FTK_ENUM(OverMode);

        //! Over node.
        class OverNode : public INode
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const nlohmann::json&);

            OverNode();

        public:
            virtual ~OverNode();

            static NodeInfo getClassNodeInfo();

            static std::shared_ptr<INode> create(
                const std::shared_ptr<ftk::Context>&,
                const nlohmann::json& = {});

            void exec(
                const std::shared_ptr<ftk::IRender>&,
                const OTIO_NS::RationalTime&) override;

        private:
            FTK_PRIVATE();
        };
    }
}
