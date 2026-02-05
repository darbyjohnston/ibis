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
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const nlohmann::json&);

            ImageFileNode();

        public:
            virtual ~ImageFileNode();

            static NodeInfo getClassNodeInfo();

            static std::vector<std::string> getExts();

            static std::shared_ptr<INode> create(
                const std::shared_ptr<ftk::Context>&,
                const nlohmann::json& = {});

            std::shared_ptr<ftk::IObservableList<std::string> > observeSubImages() const;
            std::shared_ptr<ftk::IObservableList<std::string> > observeChannels() const;

            bool setAttr(const NodeAttr&) override;

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
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const nlohmann::json&);

            ImageSequenceNode();

        public:
            virtual ~ImageSequenceNode();

            static NodeInfo getClassNodeInfo();

            static std::vector<std::string> getExts();

            static std::shared_ptr<INode> create(
                const std::shared_ptr<ftk::Context>&,
                const nlohmann::json& = {});

            std::shared_ptr<ftk::IObservableList<std::string> > observeSubImages() const;
            std::shared_ptr<ftk::IObservableList<std::string> > observeChannels() const;

            bool setAttr(const NodeAttr&) override;

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
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const nlohmann::json&);

            SVGFileNode();

        public:
            virtual ~SVGFileNode();

            static NodeInfo getClassNodeInfo();

            static std::vector<std::string> getExts();

            static std::shared_ptr<INode> create(
                const std::shared_ptr<ftk::Context>&,
                const nlohmann::json& = {});

            void exec(
                const std::shared_ptr<ftk::IRender>&,
                const OTIO_NS::RationalTime&) override;

        private:
            FTK_PRIVATE();
        };

        std::shared_ptr<INode> createInputNode(
            const std::shared_ptr<ftk::Context>&,
            const std::string& fileName);
    }
}
