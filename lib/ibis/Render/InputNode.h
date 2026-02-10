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

#if defined(IBIS_OIIO)
        //! Image input node.
        class ImageInputNode : public INode
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const nlohmann::json&);

            ImageInputNode();

        public:
            virtual ~ImageInputNode();

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

        //! Image sequence input node.
        class SequenceInputNode : public INode
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const nlohmann::json&);

            SequenceInputNode();

        public:
            virtual ~SequenceInputNode();

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
#endif // IBIS_OIIO

        //! SVG file input node.
        class SVGInputNode : public INode
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const nlohmann::json&);

            SVGInputNode();

        public:
            virtual ~SVGInputNode();

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
