// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "TimelineWidget.h"

#include <ftk/UI/Label.h>

namespace ibis
{
    namespace ui
    {
        struct TimelineWidget::Private
        {
            std::shared_ptr<ftk::Label> label;
        };

        void TimelineWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::TimelineWidget", parent);
            FTK_P();
            p.label = ftk::Label::create(context, "Timeline Widget", shared_from_this());
            p.label->setHAlign(ftk::HAlign::Center);
            p.label->setMarginRole(ftk::SizeRole::MarginLarge);
        }

        TimelineWidget::TimelineWidget() :
            _p(new Private)
        {
        }

        TimelineWidget::~TimelineWidget()
        {
        }

        std::shared_ptr<TimelineWidget> TimelineWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<TimelineWidget> out(new TimelineWidget);
            out->_init(context, parent);
            return out;
        }

        ftk::Size2I TimelineWidget::getSizeHint() const
        {
            return _p->label->getSizeHint();
        }

        void TimelineWidget::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->label->setGeometry(value);
        }
    }
}
