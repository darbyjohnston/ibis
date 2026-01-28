// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#pragma once

#include <ftk/UI/IMouseWidget.h>

#include <opentimelineio/version.h>

namespace ibis
{
    namespace models
    {
        class TimeModel;
        class TimeUnitsModel;
    }

    namespace ui
    {
        //! Time slider.
        class TimeSlider : public ftk::IMouseWidget
        {
            FTK_NON_COPYABLE(TimeSlider);

        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::TimeUnitsModel>&,
                const std::shared_ptr<models::TimeModel>&,
                const std::shared_ptr<IWidget>& parent);

            TimeSlider();

        public:
            virtual ~TimeSlider();

            //! Create a new widget.
            static std::shared_ptr<TimeSlider> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::TimeUnitsModel>&,
                const std::shared_ptr<models::TimeModel>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            ftk::Size2I getSizeHint() const override;
            void sizeHintEvent(const ftk::SizeHintEvent&) override;
            void drawEvent(const ftk::Box2I&, const ftk::DrawEvent&) override;
            void mouseMoveEvent(ftk::MouseMoveEvent&) override;
            void mousePressEvent(ftk::MouseClickEvent&) override;
            void mouseReleaseEvent(ftk::MouseClickEvent&) override;

        private:
            OTIO_NS::RationalTime _posToTime(int) const;
            int _timeToPos(const OTIO_NS::RationalTime&) const;

            FTK_PRIVATE();
        };
    }
}
