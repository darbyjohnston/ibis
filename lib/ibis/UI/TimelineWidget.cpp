// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "TimelineWidget.h"

#include <ibis/UI/TimeEdit.h>
#include <ibis/UI/TimeLabel.h>
#include <ibis/UI/TimeUnitsWidget.h>

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
        };

        void TimelineWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::TimeUnitsModel>& timeUnitsModel,
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
        }

        TimelineWidget::TimelineWidget() :
            _p(new Private)
        {}

        TimelineWidget::~TimelineWidget()
        {}

        std::shared_ptr<TimelineWidget> TimelineWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::TimeUnitsModel>& timeUnitsModel,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<TimelineWidget> out(new TimelineWidget);
            out->_init(context, timeUnitsModel, parent);
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
