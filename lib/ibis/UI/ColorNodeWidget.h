// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ibis/UI/INodeWidget.h>

namespace ibis
{
    namespace ui
    {
        //! Brightness node widget.
        class BrightnessNodeWidget : public INodeWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ibis::models::Document>&,
                const std::shared_ptr<ibis::render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent);

            BrightnessNodeWidget();

        public:
            virtual ~BrightnessNodeWidget();

            //! Create a new widget.
            static std::shared_ptr<BrightnessNodeWidget> create(
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

        //! Contrast node widget.
        class ContrastNodeWidget : public INodeWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ibis::models::Document>&,
                const std::shared_ptr<ibis::render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent);

            ContrastNodeWidget();

        public:
            virtual ~ContrastNodeWidget();

            //! Create a new widget.
            static std::shared_ptr<ContrastNodeWidget> create(
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
