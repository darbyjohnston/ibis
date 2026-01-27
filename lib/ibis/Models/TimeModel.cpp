// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <ibis/Models/TimeModel.h>

#include <ftk/Core/Error.h>
#include <ftk/Core/Format.h>
#include <ftk/Core/String.h>

#include <cstdlib>

namespace ibis
{
    namespace models
    {
        FTK_ENUM_IMPL(
            Playback,
            "Stop",
            "Forward",
            "Reverse");

        FTK_ENUM_IMPL(
            PlaybackLoop,
            "Loop",
            "Once",
            "PingPong");

        struct TimeModel::Private
        {
            std::shared_ptr<ftk::Observable<OTIO_NS::TimeRange> > timeRange;
            std::shared_ptr<ftk::Observable<OTIO_NS::RationalTime> > currentTime;
            std::shared_ptr<ftk::Observable<Playback> > playback;
        };

        void TimeModel::_init(const std::shared_ptr<ftk::Context>& context)
        {
            FTK_P();
            p.timeRange = ftk::Observable<OTIO_NS::TimeRange>::create(OTIO_NS::TimeRange(0.0, 100.0, 24.0));
            p.currentTime = ftk::Observable<OTIO_NS::RationalTime>::create(OTIO_NS::RationalTime(0.0, 24.0));
            p.playback = ftk::Observable<Playback>::create(Playback::Stop);
        }

        TimeModel::TimeModel() :
            _p(new Private)
        {}

        TimeModel::~TimeModel()
        {}

        std::shared_ptr<TimeModel> TimeModel::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            auto out = std::shared_ptr<TimeModel>(new TimeModel);
            out->_init(context);
            return out;
        }

        const OTIO_NS::TimeRange& TimeModel::getTimeRange()
        {
            return _p->timeRange->get();
        }

        std::shared_ptr<ftk::IObservable<OTIO_NS::TimeRange> > TimeModel::observeTimeRange() const
        {
            return _p->timeRange;
        }

        void TimeModel::setTimeRange(const OTIO_NS::TimeRange& value)
        {
            _p->timeRange->setIfChanged(value);
        }

        const OTIO_NS::RationalTime& TimeModel::getCurrentTime()
        {
            return _p->currentTime->get();
        }

        std::shared_ptr<ftk::IObservable<OTIO_NS::RationalTime> > TimeModel::observeCurrentTime() const
        {
            return _p->currentTime;
        }

        void TimeModel::setCurrentTime(const OTIO_NS::RationalTime& value)
        {
            _p->currentTime->setIfChanged(value);
        }

        Playback TimeModel::getPlayback() const
        {
            return _p->playback->get();
        }

        std::shared_ptr<ftk::IObservable<Playback> > TimeModel::observePlayback() const
        {
            return _p->playback;
        }

        void TimeModel::setPlayback(Playback value)
        {
            _p->playback->setIfChanged(value);
        }
    }
}
