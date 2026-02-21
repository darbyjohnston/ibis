// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ibis/UI/NodeDragDrop.h>

#include <ibis/Render/NodeFactory.h>

#include <ftk/UI/IMouseWidget.h>

namespace ftk
{
    class SysLogModel;
}

namespace ibis
{
    namespace models
    {
        class SettingsModel;
    }

    namespace ui
    {
        //! Messages widget.
        class MessagesWidget : public ftk::IWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::SettingsModel>&,
                const std::shared_ptr<ftk::SysLogModel>&,
                const std::shared_ptr<ftk::IWidget>& parent);

            MessagesWidget();

        public:
            virtual ~MessagesWidget();

            //! Create a new widget.
            static std::shared_ptr<MessagesWidget> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::SettingsModel>&,
                const std::shared_ptr<ftk::SysLogModel>&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;

        private:
            FTK_PRIVATE();
        };
    }
}
