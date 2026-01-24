// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ibis/UI/INodeWidget.h>

namespace ibis
{
    namespace ui
    {
        //! Add value node widget.
        class AddValueNodeWidget : public INodeWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ibis::render::Graph>&,
                const std::shared_ptr<ibis::render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent);

            AddValueNodeWidget();

        public:
            virtual ~AddValueNodeWidget();

            //! Create a new widget.
            static std::shared_ptr<AddValueNodeWidget> create(
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
