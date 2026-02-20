// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include <ibis/UI/TimeSlider.h>

#include <ibis/Models/TimeModel.h>
#include <ibis/Models/TimeUnitsModel.h>

#include <optional>

namespace ibis
{
    namespace ui
    {
        struct TimeSlider::Private
        {
            std::shared_ptr<models::TimeUnitsModel> timeUnitsModel;
            std::shared_ptr<models::TimeModel> timeModel;

            struct SizeData
            {
                std::optional<float> displayScale;
                int margin = 0;
                int border = 0;
                ftk::FontInfo fontInfo;
                ftk::FontMetrics fontMetrics;
            };
            SizeData size;

            std::shared_ptr<ftk::Observer<models::TimeUnits> > timeUnitsObserver;
            std::shared_ptr<ftk::Observer<OTIO_NS::TimeRange> > timeRangeObserver;
            std::shared_ptr<ftk::Observer<OTIO_NS::RationalTime> > currentTimeObserver;
        };

        void TimeSlider::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::TimeUnitsModel>& timeUnitsModel,
            const std::shared_ptr<models::TimeModel>& timeModel,
            const std::shared_ptr<IWidget>& parent)
        {
            IMouseWidget::_init(context, "tl::ui::TimeSlider", parent);
            FTK_P();

            _setMouseHoverEnabled(true);
            _setMousePressEnabled(true);

            p.timeUnitsModel = timeUnitsModel;
            p.timeModel = timeModel;

            p.timeUnitsObserver = ftk::Observer<models::TimeUnits>::create(
                p.timeUnitsModel->observeTimeUnits(),
                [this](models::TimeUnits)
                {
                });

            p.timeRangeObserver = ftk::Observer<OTIO_NS::TimeRange> ::create(
                p.timeModel->observeTimeRange(),
                [this](const OTIO_NS::TimeRange& value)
                {
                });

            p.currentTimeObserver = ftk::Observer<OTIO_NS::RationalTime>::create(
                p.timeModel->observeCurrentTime(),
                [this](const OTIO_NS::RationalTime& value)
                {
                });
        }

        TimeSlider::TimeSlider() :
            _p(new Private)
        {}

        TimeSlider::~TimeSlider()
        {}

        std::shared_ptr<TimeSlider> TimeSlider::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::TimeUnitsModel>& timeUnitsModel,
            const std::shared_ptr<models::TimeModel>& timeModel,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<TimeSlider>(new TimeSlider);
            out->_init(context, timeUnitsModel, timeModel, parent);
            return out;
        }
        
        ftk::Size2I TimeSlider::getSizeHint() const
        {
            FTK_P();
            ftk::Size2I out;
            out.h =
                p.size.fontMetrics.lineHeight +
                p.size.margin * 4;
            return out;
        }

        void TimeSlider::sizeHintEvent(const ftk::SizeHintEvent& event)
        {
            FTK_P();
            if (!p.size.displayScale.has_value() ||
                (p.size.displayScale.has_value() && p.size.displayScale.value() != event.displayScale))
            {
                p.size.displayScale = event.displayScale;
                p.size.margin = event.style->getSizeRole(ftk::SizeRole::MarginInside, event.displayScale);
                p.size.border = event.style->getSizeRole(ftk::SizeRole::Border, event.displayScale);
                p.size.fontInfo = event.style->getFontRole(ftk::FontRole::Mono, event.displayScale);
                p.size.fontMetrics = event.fontSystem->getMetrics(p.size.fontInfo);
            }
        }

        void TimeSlider::drawEvent(
            const ftk::Box2I& drawRect,
            const ftk::DrawEvent& event)
        {
            FTK_P();

            // Draw the background.
            const ftk::Box2I& g = getGeometry();
            event.render->drawRect(g, event.style->getColorRole(ftk::ColorRole::Base));

            // Draw the frames.
            const ftk::Box2I g2 = ftk::margin(g, -p.size.margin, 0, -p.size.margin, 0);
            const OTIO_NS::TimeRange& timeRange = p.timeModel->getTimeRange();
            int l = _timeToPos(timeRange.start_time() + OTIO_NS::RationalTime(1.0, timeRange.duration().rate())) -
                _timeToPos(timeRange.start_time());
            if (l > p.size.margin * 2)
            {
                for (int64_t f = timeRange.start_time().value();
                    f <= timeRange.end_time_inclusive().value();
                    ++f)
                {
                    OTIO_NS::RationalTime t = OTIO_NS::RationalTime(f, timeRange.duration().rate());
                    int x = _timeToPos(t);
                    if (x >= g2.min.x)
                    {
                        event.render->drawRect(
                            ftk::Box2I(x, g2.min.y + g2.h() - p.size.margin * 2, p.size.border, p.size.margin * 2),
                            event.style->getColorRole(ftk::ColorRole::Border));
                    }
                }
            }

            // Draw the seconds.
            l = _timeToPos(timeRange.start_time() + OTIO_NS::RationalTime(1.0, 1.0)) -
                _timeToPos(timeRange.start_time());
            for (int64_t s = timeRange.start_time().to_seconds();
                s <= timeRange.end_time_inclusive().to_seconds();
                ++s)
            {
                OTIO_NS::RationalTime t = OTIO_NS::RationalTime(s, 1.0).rescaled_to(timeRange.duration().rate());
                int x = _timeToPos(t);
                std::string text = p.timeUnitsModel->getLabel(t);
                const ftk::Size2I textSize = event.fontSystem->getSize(text, p.size.fontInfo);
                if (x >= g2.min.x)
                {
                    event.render->drawRect(
                        ftk::Box2I(x, g2.min.y, p.size.border, g2.h()),
                        event.style->getColorRole(ftk::ColorRole::Border));
                    if (p.size.border + p.size.margin * 4 + textSize.w < l)
                    {
                        x += p.size.border + p.size.margin * 2;
                        event.render->drawText(
                            event.fontSystem->getGlyphs(text, p.size.fontInfo),
                            p.size.fontMetrics,
                            ftk::V2I(x, g2.min.y + p.size.margin),
                            event.style->getColorRole(ftk::ColorRole::TextDisabled));
                    }
                }
            }

            // Draw the current time.
            OTIO_NS::RationalTime t = p.timeModel->getCurrentTime();
            int x = _timeToPos(t);
            event.render->drawRect(
                ftk::Box2I(x - p.size.border, g2.min.y, p.size.border * 2, g2.h()),
                event.style->getColorRole(ftk::ColorRole::Red));
            int m = p.size.border + p.size.margin * 2;
            std::string text = p.timeUnitsModel->getLabel(t);
            const ftk::Size2I textSize = event.fontSystem->getSize(text, p.size.fontInfo);
            if (x + m + textSize.w < g2.min.x + g2.w())
            {
                x += m;
            }
            else if (x - m - textSize.w >= g2.min.x)
            {
                x -= m + textSize.w;
            }
            event.render->drawText(
                event.fontSystem->getGlyphs(text, p.size.fontInfo),
                p.size.fontMetrics,
                ftk::V2I(x, g2.min.y + p.size.margin),
                event.style->getColorRole(ftk::ColorRole::Text));
        }

        void TimeSlider::mouseMoveEvent(ftk::MouseMoveEvent& event)
        {
            IMouseWidget::mouseMoveEvent(event);
            FTK_P();
            if (_isMousePressed())
            {
                const OTIO_NS::RationalTime t = _posToTime(event.pos.x);
                p.timeModel->setCurrentTime(t);
            }
        }

        void TimeSlider::mousePressEvent(ftk::MouseClickEvent& event)
        {
            IMouseWidget::mousePressEvent(event);
            FTK_P();
            const OTIO_NS::RationalTime t = _posToTime(event.pos.x);
            p.timeModel->setCurrentTime(t);
        }

        void TimeSlider::mouseReleaseEvent(ftk::MouseClickEvent& event)
        {
            IMouseWidget::mouseReleaseEvent(event);
        }

        OTIO_NS::RationalTime TimeSlider::_posToTime(int pos) const
        {
            FTK_P();
            const ftk::Box2I& g = getGeometry();
            const ftk::Box2I g2 = ftk::margin(g, -p.size.margin, 0, -p.size.margin, 0);
            const float v = (pos - g2.min.x) / static_cast<float>(g2.w() - 1);
            const OTIO_NS::TimeRange& timeRange = p.timeModel->getTimeRange();
            OTIO_NS::RationalTime t = timeRange.start_time() +
                OTIO_NS::RationalTime(
                    v * (timeRange.end_time_inclusive().value() - timeRange.start_time().value()),
                    timeRange.duration().rate());
            return timeRange.clamped(t).floor();
        }

        int TimeSlider::_timeToPos(const OTIO_NS::RationalTime& time) const
        {
            FTK_P();
            const OTIO_NS::TimeRange& timeRange = p.timeModel->getTimeRange();
            const float v = (time - timeRange.start_time()).value() /
                (timeRange.end_time_inclusive().value() - timeRange.start_time().value());
            const ftk::Box2I& g = getGeometry();
            const ftk::Box2I g2 = ftk::margin(g, -p.size.margin, 0, -p.size.margin, 0);
            return g2.min.x + v * (g2.w() - 1);
        }
    }
}
