// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ibis/UI/INodeWidget.h>

namespace ibis
{
    namespace ui
    {
        //! Over node widget.
        class OverNodeWidget : public IInteractionNodeWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::Document>&,
                const std::shared_ptr<render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent);

            OverNodeWidget();

        public:
            virtual ~OverNodeWidget();

            //! Create a new widget.
            static std::shared_ptr<OverNodeWidget> create(
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
