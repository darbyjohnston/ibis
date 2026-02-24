// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ibis/Render/INode.h>

#include <ftk/Core/Matrix.h>

namespace ibis
{
    namespace render
    {
        //! Base class for color matrix nodes.
        class IColorMatrixNode : public INode
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const NodeInfo& info,
                int inputCount,
                int outputCount,
                const NodeAttr&,
                const nlohmann::json&);

            IColorMatrixNode();

        public:
            virtual ~IColorMatrixNode() = 0;

            void exec(
                const std::shared_ptr<ftk::IRender>&,
                const OTIO_NS::RationalTime&) override;

        protected:
            virtual ftk::M44F _getColorMatrix() const = 0;

        private:
            FTK_PRIVATE();
        };

        //! Brightness node.
        class BrightnessNode : public IColorMatrixNode
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const nlohmann::json&);

            BrightnessNode() = default;

        public:
            virtual ~BrightnessNode();

            static NodeInfo getClassNodeInfo();

            static std::shared_ptr<INode> create(
                const std::shared_ptr<ftk::Context>&,
                const nlohmann::json& = {});

        protected:
            ftk::M44F _getColorMatrix() const override;
        };

        //! Contrast node.
        class ContrastNode : public IColorMatrixNode
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const nlohmann::json&);

            ContrastNode() = default;

        public:
            virtual ~ContrastNode();

            static NodeInfo getClassNodeInfo();

            static std::shared_ptr<INode> create(
                const std::shared_ptr<ftk::Context>&,
                const nlohmann::json& = {});

        protected:
            ftk::M44F _getColorMatrix() const override;
        };

        //! Saturation node.
        class SaturationNode : public IColorMatrixNode
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const nlohmann::json&);

            SaturationNode() = default;

        public:
            virtual ~SaturationNode();

            static NodeInfo getClassNodeInfo();

            static std::shared_ptr<INode> create(
                const std::shared_ptr<ftk::Context>&,
                const nlohmann::json& = {});

        protected:
            ftk::M44F _getColorMatrix() const override;
        };

        //! Color hue node.
        class HueNode : public IColorMatrixNode
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const nlohmann::json&);

            HueNode() = default;

        public:
            virtual ~HueNode();

            static NodeInfo getClassNodeInfo();

            static std::shared_ptr<INode> create(
                const std::shared_ptr<ftk::Context>&,
                const nlohmann::json& = {});

        protected:
            ftk::M44F _getColorMatrix() const override;
        };

        //! Invert node.
        class InvertNode : public INode
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const nlohmann::json&);

            InvertNode();

        public:
            virtual ~InvertNode();

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

        //! Levels node.
        class LevelsNode : public INode
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const nlohmann::json&);

            LevelsNode();

        public:
            virtual ~LevelsNode();

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

        //! Soft clip node.
        class SoftClipNode : public INode
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const nlohmann::json&);

            SoftClipNode();

        public:
            virtual ~SoftClipNode();

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
