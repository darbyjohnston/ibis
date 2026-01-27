// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <ibis/Models/TimeUnitsModel.h>

#include <ftk/UI/IWidget.h>

namespace ibis
{
    namespace ui
    {
        //! Time units widget.
        class TimeUnitsWidget : public ftk::IWidget
        {
            FTK_NON_COPYABLE(TimeUnitsWidget);

        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::TimeUnitsModel>&,
                const std::shared_ptr<IWidget>& parent);

            TimeUnitsWidget();

        public:
            virtual ~TimeUnitsWidget();

            //! Create a new widget.
            static std::shared_ptr<TimeUnitsWidget> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::TimeUnitsModel>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;

        private:
            void _showPopup();

            FTK_PRIVATE();
        };
    }
}
