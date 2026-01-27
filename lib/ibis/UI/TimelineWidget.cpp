// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "TimelineWidget.h"

#include <ibis/UI/TimeEdit.h>
#include <ibis/UI/TimeLabel.h>
#include <ibis/UI/TimeUnitsWidget.h>

#include <ibis/Models/Document.h>
#include <ibis/Models/TimeModel.h>

#include <ftk/UI/RowLayout.h>

namespace ibis
{
    namespace ui
    {
        struct TimelineWidget::Private
        {
            std::shared_ptr<ui::TimeEdit> currentTimeEdit;
            std::shared_ptr<ui::TimeEdit> startTimeEdit;
            std::shared_ptr<ui::TimeEdit> durationEdit;
            std::shared_ptr<ui::TimeUnitsWidget> timeUnitsWidget;
            std::shared_ptr<ftk::HorizontalLayout> layout;

            std::shared_ptr<ftk::Observer<OTIO_NS::TimeRange> > timeRangeObserver;
            std::shared_ptr<ftk::Observer<OTIO_NS::RationalTime> > currentTimeObserver;
        };

        void TimelineWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::TimeUnitsModel>& timeUnitsModel,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::TimelineWidget", parent);
            FTK_P();

            p.currentTimeEdit = ui::TimeEdit::create(context, timeUnitsModel);
            p.startTimeEdit = ui::TimeEdit::create(context, timeUnitsModel);
            p.durationEdit = ui::TimeEdit::create(context, timeUnitsModel);
            p.timeUnitsWidget = ui::TimeUnitsWidget::create(context, timeUnitsModel);

            p.layout = ftk::HorizontalLayout::create(context, shared_from_this());
            p.layout->setMarginRole(ftk::SizeRole::MarginSmall);
            p.layout->setSpacingRole(ftk::SizeRole::MarginSmall);
            p.currentTimeEdit->setParent(p.layout);
            p.startTimeEdit->setParent(p.layout);
            p.layout->addSpacer(ftk::Stretch::Expanding);
            p.durationEdit->setParent(p.layout);
            p.timeUnitsWidget->setParent(p.layout);

            p.currentTimeEdit->setCallback(
                [document](const OTIO_NS::RationalTime& value)
                {
                    document->getTimeModel()->setCurrentTime(value);
                });

            p.startTimeEdit->setCallback(
                [document](const OTIO_NS::RationalTime& value)
                {
                    auto timeModel = document->getTimeModel();
                    const OTIO_NS::TimeRange timeRange = timeModel->getTimeRange();
                    timeModel->setTimeRange(OTIO_NS::TimeRange(value, timeRange.duration()));
                });

            p.durationEdit->setCallback(
                [document](const OTIO_NS::RationalTime& value)
                {
                    auto timeModel = document->getTimeModel();
                    const OTIO_NS::TimeRange timeRange = timeModel->getTimeRange();
                    timeModel->setTimeRange(OTIO_NS::TimeRange(timeRange.start_time(), value));
                });

            auto timeModel = document->getTimeModel();
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
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<TimelineWidget> out(new TimelineWidget);
            out->_init(context, timeUnitsModel, document, parent);
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
