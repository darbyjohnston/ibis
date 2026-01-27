// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ftk/UI/IWidget.h>

namespace ibis
{
    namespace models
    {
        class TimeModel;
        class TimeUnitsModel;
    }

    namespace ui
    {
        //! Timeline widget.
        class TimelineWidget : public ftk::IWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::TimeUnitsModel>&,
                const std::shared_ptr<models::TimeModel>&,
                const std::shared_ptr<ftk::IWidget>& parent);

            TimelineWidget();

        public:
            virtual ~TimelineWidget();

            //! Create a new widget.
            static std::shared_ptr<TimelineWidget> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::TimeUnitsModel>&,
                const std::shared_ptr<models::TimeModel>&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;

        private:
            FTK_PRIVATE();
        };
    }
}
