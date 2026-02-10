// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ibis/UI/INodeWidget.h>

namespace ibis
{
    namespace ui
    {
        //! Image output node widget.
        class ImageOutputNodeWidget : public INodeWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::Document>&,
                const std::shared_ptr<render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent);

            ImageOutputNodeWidget();

        public:
            virtual ~ImageOutputNodeWidget();

            //! Create a new widget.
            static std::shared_ptr<ImageOutputNodeWidget> create(
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

        //! Sequence output node widget.
        class SequenceOutputNodeWidget : public INodeWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::Document>&,
                const std::shared_ptr<render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent);

            SequenceOutputNodeWidget();

        public:
            virtual ~SequenceOutputNodeWidget();

            //! Create a new widget.
            static std::shared_ptr<SequenceOutputNodeWidget> create(
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
