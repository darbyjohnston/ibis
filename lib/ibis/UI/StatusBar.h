// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#pragma once

#include <ftk/UI/IMouseWidget.h>

namespace ibis
{
    namespace models
    {
        class MessagesModel;
    }

    namespace ui
    {
        //! Status bar.
        class StatusBar : public ftk::IMouseWidget
        {
            FTK_NON_COPYABLE(StatusBar);

        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::MessagesModel>&,
                const std::shared_ptr<IWidget>& parent);

            StatusBar();

        public:
            virtual ~StatusBar();

            //! Create a new widget.
            static std::shared_ptr<StatusBar> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::MessagesModel>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            void setCallback(const std::function<void(void)>&);

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;
            void mousePressEvent(ftk::MouseClickEvent&) override;

        private:
            FTK_PRIVATE();
        };
    }
}
