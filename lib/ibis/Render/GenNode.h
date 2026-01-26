// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ibis/Render/INode.h>

namespace ibis
{
    namespace render
    {
        //! Solid color node.
        class SolidColorNode : public INode
        {
        protected:
            void _init(const std::shared_ptr<ftk::Context>&);

            SolidColorNode();

        public:
            virtual ~SolidColorNode();

            static std::string getNodeID();

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
