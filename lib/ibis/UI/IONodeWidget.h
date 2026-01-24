// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ibis/UI/INodeWidget.h>

namespace ibis
{
    namespace ui
    {
        //! Input node widget.
        class InputNodeWidget : public INodeWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ibis::render::Graph>&,
                const std::shared_ptr<ibis::render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent);

            InputNodeWidget();

        public:
            virtual ~InputNodeWidget();

            //! Create a new widget.
            static std::shared_ptr<InputNodeWidget> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ibis::render::Graph>&,
                const std::shared_ptr<ibis::render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);

            static std::string getNodeID();

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;

        private:
            FTK_PRIVATE();
        };

        //! Output node widget.
        class OutputNodeWidget : public INodeWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ibis::render::Graph>&,
                const std::shared_ptr<ibis::render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent);

            OutputNodeWidget();

        public:
            virtual ~OutputNodeWidget();

            //! Create a new widget.
            static std::shared_ptr<OutputNodeWidget> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ibis::render::Graph>&,
                const std::shared_ptr<ibis::render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);

            static std::string getNodeID();

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;

        private:
            FTK_PRIVATE();
        };
    }
}
