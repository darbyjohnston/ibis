// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ibis/UI/INodeWidget.h>

namespace ibis
{
    namespace ui
    {
        //! Image file node widget.
        class ImageFileNodeWidget : public INodeWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ibis::models::Document>&,
                const std::shared_ptr<ibis::render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent);

            ImageFileNodeWidget();

        public:
            virtual ~ImageFileNodeWidget();

            //! Create a new widget.
            static std::shared_ptr<ImageFileNodeWidget> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ibis::models::Document>&,
                const std::shared_ptr<ibis::render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);

            static render::NodeInfo getNodeInfo();

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;

        private:
            FTK_PRIVATE();
        };

        //! Image sequence node widget.
        class ImageSequenceNodeWidget : public INodeWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ibis::models::Document>&,
                const std::shared_ptr<ibis::render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent);

            ImageSequenceNodeWidget();

        public:
            virtual ~ImageSequenceNodeWidget();

            //! Create a new widget.
            static std::shared_ptr<ImageSequenceNodeWidget> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ibis::models::Document>&,
                const std::shared_ptr<ibis::render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);

            static render::NodeInfo getNodeInfo();

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;

        private:
            FTK_PRIVATE();
        };

        //! SVG file node widget.
        class SVGFileNodeWidget : public INodeWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ibis::models::Document>&,
                const std::shared_ptr<ibis::render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent);

            SVGFileNodeWidget();

        public:
            virtual ~SVGFileNodeWidget();

            //! Create a new widget.
            static std::shared_ptr<SVGFileNodeWidget> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ibis::models::Document>&,
                const std::shared_ptr<ibis::render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);

            static render::NodeInfo getNodeInfo();

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;

        private:
            FTK_PRIVATE();
        };
    }
}
