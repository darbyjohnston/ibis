// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "TimelineWidget.h"

#include <ibis/UI/FrameToolBar.h>
#include <ibis/UI/PlaybackToolBar.h>
#include <ibis/UI/TimeEdit.h>
#include <ibis/UI/TimeLabel.h>
#include <ibis/UI/TimeSlider.h>
#include <ibis/UI/TimeUnitsWidget.h>

#include <ibis/Models/TimeModel.h>

#include <ftk/UI/Divider.h>
#include <ftk/UI/RowLayout.h>

namespace ibis
{
    namespace ui
    {
        struct TimelineWidget::Private
        {
            std::shared_ptr<ui::TimeSlider> timeSlider;
            std::shared_ptr<ui::PlaybackToolBar> playbackToolBar;
            std::shared_ptr<ui::FrameToolBar> frameToolBar;
            std::shared_ptr<ui::TimeEdit> currentTimeEdit;
            std::shared_ptr<ui::TimeEdit> startTimeEdit;
            std::shared_ptr<ui::TimeEdit> durationEdit;
            std::shared_ptr<ui::TimeUnitsWidget> timeUnitsWidget;
            std::shared_ptr<ftk::VerticalLayout> layout;

            std::shared_ptr<ftk::Observer<OTIO_NS::TimeRange> > timeRangeObserver;
            std::shared_ptr<ftk::Observer<OTIO_NS::RationalTime> > currentTimeObserver;
        };

        void TimelineWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::TimeUnitsModel>& timeUnitsModel,
            const std::shared_ptr<models::TimeModel>& timeModel,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::TimelineWidget", parent);
            FTK_P();

            p.timeSlider = ui::TimeSlider::create(context, timeUnitsModel, timeModel);
            p.timeSlider->setTooltip("Time slider.");

            p.playbackToolBar = ui::PlaybackToolBar::create(context, timeModel);

            p.frameToolBar = ui::FrameToolBar::create(context, timeModel);

            p.currentTimeEdit = ui::TimeEdit::create(context, timeUnitsModel);
            p.currentTimeEdit->setTooltip("Current time.");

            p.startTimeEdit = ui::TimeEdit::create(context, timeUnitsModel);
            p.startTimeEdit->setTooltip("Start time.");

            p.durationEdit = ui::TimeEdit::create(context, timeUnitsModel);
            p.durationEdit->setTooltip("Time duration.");

            p.timeUnitsWidget = ui::TimeUnitsWidget::create(context, timeUnitsModel);
            p.timeUnitsWidget->setTooltip("Time units.");

            p.layout = ftk::VerticalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(ftk::SizeRole::None);
            p.timeSlider->setParent(p.layout);
            ftk::Divider::create(context, ftk::Orientation::Vertical, p.layout);
            auto hLayout = ftk::HorizontalLayout::create(context, p.layout);
            hLayout->setMarginRole(ftk::SizeRole::MarginInside);
            hLayout->setSpacingRole(ftk::SizeRole::MarginSmall);
            p.playbackToolBar->setParent(hLayout);
            p.frameToolBar->setParent(hLayout);
            p.currentTimeEdit->setParent(hLayout);
            p.startTimeEdit->setParent(hLayout);
            p.layout->addSpacer(ftk::Stretch::Expanding);
            p.durationEdit->setParent(hLayout);
            p.timeUnitsWidget->setParent(hLayout);

            p.currentTimeEdit->setCallback(
                [timeModel](const OTIO_NS::RationalTime& value)
                {
                    timeModel->setCurrentTime(value);
                });

            p.startTimeEdit->setCallback(
                [timeModel](const OTIO_NS::RationalTime& value)
                {
                    const OTIO_NS::TimeRange timeRange = timeModel->getTimeRange();
                    timeModel->setTimeRange(OTIO_NS::TimeRange(value, timeRange.duration()));
                });

            p.durationEdit->setCallback(
                [timeModel](const OTIO_NS::RationalTime& value)
                {
                    const OTIO_NS::TimeRange timeRange = timeModel->getTimeRange();
                    timeModel->setTimeRange(OTIO_NS::TimeRange(timeRange.start_time(), value));
                });

            p.timeRangeObserver = ftk::Observer<OTIO_NS::TimeRange>::create(
                timeModel->observeTimeRange(),
                [this](const OTIO_NS::TimeRange& value)
                {
                    FTK_P();
                    p.startTimeEdit->setValue(value.start_time());
                    p.durationEdit->setValue(value.duration());
                });

            p.currentTimeObserver = ftk::Observer<OTIO_NS::RationalTime>::create(
                timeModel->observeCurrentTime(),
                [this](const OTIO_NS::RationalTime& value)
                {
                    FTK_P();
                    p.currentTimeEdit->setValue(value);
                });
        }

        TimelineWidget::TimelineWidget() :
            _p(new Private)
        {}

        TimelineWidget::~TimelineWidget()
        {}

        std::shared_ptr<TimelineWidget> TimelineWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::TimeUnitsModel>& timeUnitsModel,
            const std::shared_ptr<models::TimeModel>& timeModel,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<TimelineWidget> out(new TimelineWidget);
            out->_init(context, timeUnitsModel, timeModel, parent);
            return out;
        }

        ftk::Size2I TimelineWidget::getSizeHint() const
        {
            return _p->layout->getSizeHint();
        }

        void TimelineWidget::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }
    }
}
