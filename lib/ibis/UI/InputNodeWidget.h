// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ibis/UI/INodeWidget.h>

namespace ibis
{
    namespace ui
    {
        //! Get a tooltip for the supported file extensions.
        std::string getExtTooltip(const std::vector<std::string>& exts);

        //! Image input node widget.
        class ImageInputNodeWidget : public INodeWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::Document>&,
                const std::shared_ptr<render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent);

            ImageInputNodeWidget();

        public:
            virtual ~ImageInputNodeWidget();

            //! Create a new widget.
            static std::shared_ptr<ImageInputNodeWidget> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::Document>&,
                const std::shared_ptr<render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);

            static render::NodeInfo getClassNodeInfo();

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;

        private:
            FTK_PRIVATE();
        };

        //! Image sequence input node widget.
        class SequenceInputNodeWidget : public INodeWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::Document>&,
                const std::shared_ptr<render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent);

            SequenceInputNodeWidget();

        public:
            virtual ~SequenceInputNodeWidget();

            //! Create a new widget.
            static std::shared_ptr<SequenceInputNodeWidget> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::Document>&,
                const std::shared_ptr<render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);

            static render::NodeInfo getClassNodeInfo();

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;

        private:
            FTK_PRIVATE();
        };

        //! SVG file input node widget.
        class SVGInputNodeWidget : public INodeWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::Document>&,
                const std::shared_ptr<render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent);

            SVGInputNodeWidget();

        public:
            virtual ~SVGInputNodeWidget();

            //! Create a new widget.
            static std::shared_ptr<SVGInputNodeWidget> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::Document>&,
                const std::shared_ptr<render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);

            static render::NodeInfo getClassNodeInfo();

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;

        private:
            FTK_PRIVATE();
        };
    }
}
