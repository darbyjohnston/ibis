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

        private:
            FTK_PRIVATE();
        };

        //! Output node.
        class OutputNode : public INode
        {
        protected:
            void _init(const std::shared_ptr<ftk::Context>&);

            OutputNode();

        public:
            virtual ~OutputNode();

            static std::string getNodeID();

            static std::shared_ptr<INode> create(
                const std::shared_ptr<ftk::Context>&);

        private:
            FTK_PRIVATE();
        };
    }
}
