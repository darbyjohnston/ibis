// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ibis/Render/INode.h>

namespace ibis
{
    namespace render
    {
        //! Input loop mode.
        enum class InputLoop
        {
            None,
            Loop,
            Clamp,

            Count,
            First = None
        };
        FTK_ENUM(InputLoop);

        //! Loop the input time.
        OTIO_NS::RationalTime getInputLoop(
            InputLoop,
            const OTIO_NS::RationalTime&,
            const OTIO_NS::TimeRange&);

        //! Image file node.
        class ImageFileNode : public INode
        {
        protected:
            void _init(const std::shared_ptr<ftk::Context>&);

            ImageFileNode();

        public:
            virtual ~ImageFileNode();

            static NodeInfo getNodeInfo();

            static std::shared_ptr<INode> create(
                const std::shared_ptr<ftk::Context>&);

            void exec(
                const std::shared_ptr<ftk::IRender>&,
                const OTIO_NS::RationalTime&) override;

        private:
            FTK_PRIVATE();
        };

        //! Image sequence node.
        class ImageSequenceNode : public INode
        {
        protected:
            void _init(const std::shared_ptr<ftk::Context>&);

            ImageSequenceNode();

        public:
            virtual ~ImageSequenceNode();

            static NodeInfo getNodeInfo();

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
