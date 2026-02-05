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
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const nlohmann::json&);

            ResizeNode();

        public:
            virtual ~ResizeNode();

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

        //! Crop node.
        class CropNode : public INode
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const nlohmann::json&);

            CropNode();

        public:
            virtual ~CropNode();

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

        //! Mirror node.
        class MirrorNode : public INode
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const nlohmann::json&);

            MirrorNode();

        public:
            virtual ~MirrorNode();

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

        //! Rotate node.
        class RotateNode : public INode
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const nlohmann::json&);

            RotateNode();

        public:
            virtual ~RotateNode();

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
