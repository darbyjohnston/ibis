// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ibis/Render/INode.h>

namespace ibis
{
    namespace render
    {
        //! Base class for output nodes.
        class IOutputNode : public INode
        {
        protected:
            IOutputNode() = default;

        public:
            virtual ~IOutputNode() = 0;

            virtual void write(const OTIO_NS::RationalTime&) = 0;
        };

        //! Image output node.
        class ImageOutputNode : public IOutputNode
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const nlohmann::json&);

            ImageOutputNode();

        public:
            virtual ~ImageOutputNode();

            static NodeInfo getClassNodeInfo();

            static std::vector<std::string> getExts();

            static std::shared_ptr<INode> create(
                const std::shared_ptr<ftk::Context>&,
                const nlohmann::json & = {});
            
            void write(const OTIO_NS::RationalTime&) override;

            void exec(
                const std::shared_ptr<ftk::IRender>&,
                const OTIO_NS::RationalTime&) override;

        private:
            FTK_PRIVATE();
        };

        //! Sequence output node.
        class SequenceOutputNode : public IOutputNode
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const nlohmann::json&);

            SequenceOutputNode();

        public:
            virtual ~SequenceOutputNode();

            static NodeInfo getClassNodeInfo();

            static std::vector<std::string> getExts();

            static std::shared_ptr<INode> create(
                const std::shared_ptr<ftk::Context>&,
                const nlohmann::json & = {});

            void write(const OTIO_NS::RationalTime&) override;

            void exec(
                const std::shared_ptr<ftk::IRender>&,
                const OTIO_NS::RationalTime&) override;

        private:
            FTK_PRIVATE();
        };
    }
}
