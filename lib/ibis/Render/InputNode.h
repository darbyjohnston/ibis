// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ibis/Render/INode.h>

namespace ibis
{
    namespace render
    {
        //! Image file node.
        class ImageFileNode : public INode
        {
        protected:
            void _init(const std::shared_ptr<ftk::Context>&);

            ImageFileNode();

        public:
            virtual ~ImageFileNode();

            static std::string getNodeID();

            static std::shared_ptr<INode> create(
                const std::shared_ptr<ftk::Context>&);

            void exec(
                const std::shared_ptr<ftk::IRender>&,
                const OTIO_NS::RationalTime&) override;

        private:
            FTK_PRIVATE();
        };

        //! Image file sequence node.
        class ImageFileSequenceNode : public INode
        {
        protected:
            void _init(const std::shared_ptr<ftk::Context>&);

            ImageFileSequenceNode();

        public:
            virtual ~ImageFileSequenceNode();

            static std::string getNodeID();

            static std::shared_ptr<INode> create(
                const std::shared_ptr<ftk::Context>&);

            void exec(
                const std::shared_ptr<ftk::IRender>&,
                const OTIO_NS::RationalTime&) override;

        private:
            FTK_PRIVATE();
        };

        //! SVG file node.
        class SVGFileNode : public INode
        {
        protected:
            void _init(const std::shared_ptr<ftk::Context>&);

            SVGFileNode();

        public:
            virtual ~SVGFileNode();

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
