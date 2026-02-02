// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ibis/Render/INode.h>

namespace ibis
{
    namespace render
    {
        //! Resize node.
        class ResizeNode : public INode
        {
        protected:
            void _init(const std::shared_ptr<ftk::Context>&);

            ResizeNode();

        public:
            virtual ~ResizeNode();

            static NodeInfo getClassNodeInfo();

            static std::shared_ptr<INode> create(
                const std::shared_ptr<ftk::Context>&);

            void exec(
                const std::shared_ptr<ftk::IRender>&,
                const OTIO_NS::RationalTime&) override;

        private:
            FTK_PRIVATE();
        };

        //! Crop node.
        class CropNode : public INode
        {
        protected:
            void _init(const std::shared_ptr<ftk::Context>&);

            CropNode();

        public:
            virtual ~CropNode();

            static NodeInfo getClassNodeInfo();

            static std::shared_ptr<INode> create(
                const std::shared_ptr<ftk::Context>&);

            void exec(
                const std::shared_ptr<ftk::IRender>&,
                const OTIO_NS::RationalTime&) override;

        private:
            FTK_PRIVATE();
        };

        //! Mirror node.
        class MirrorNode : public INode
        {
        protected:
            void _init(const std::shared_ptr<ftk::Context>&);

            MirrorNode();

        public:
            virtual ~MirrorNode();

            static NodeInfo getClassNodeInfo();

            static std::shared_ptr<INode> create(
                const std::shared_ptr<ftk::Context>&);

            void exec(
                const std::shared_ptr<ftk::IRender>&,
                const OTIO_NS::RationalTime&) override;

        private:
            FTK_PRIVATE();
        };

        //! Rotate node.
        class RotateNode : public INode
        {
        protected:
            void _init(const std::shared_ptr<ftk::Context>&);

            RotateNode();

        public:
            virtual ~RotateNode();

            static NodeInfo getClassNodeInfo();

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
