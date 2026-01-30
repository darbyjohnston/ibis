// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ibis/Render/INode.h>

#include <ftk/Core/Matrix.h>

namespace ibis
{
    namespace render
    {
        //! Brightness node.
        class BrightnessNode : public INode
        {
        protected:
            void _init(const std::shared_ptr<ftk::Context>&);

            BrightnessNode();

        public:
            virtual ~BrightnessNode();

            static NodeInfo getNodeInfo();

            static std::shared_ptr<INode> create(
                const std::shared_ptr<ftk::Context>&);

            void exec(
                const std::shared_ptr<ftk::IRender>&,
                const OTIO_NS::RationalTime&) override;

        private:
            FTK_PRIVATE();
        };

        //! Contrast node.
        class ContrastNode : public INode
        {
        protected:
            void _init(const std::shared_ptr<ftk::Context>&);

            ContrastNode();

        public:
            virtual ~ContrastNode();

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
