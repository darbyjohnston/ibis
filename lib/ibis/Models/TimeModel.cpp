// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include <ibis/Models/TimeModel.h>

#include <ftk/Core/Error.h>
#include <ftk/Core/Format.h>
#include <ftk/Core/String.h>
#include <ftk/Core/Timer.h>

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
            Playback togglePlayback = Playback::Forward;
            std::shared_ptr<ftk::Observable<PlaybackLoop> > loop;

            std::shared_ptr<ftk::Timer> playbackTimer;
        };

        void TimeModel::_init(const std::shared_ptr<ftk::Context>& context)
        {
            FTK_P();

            p.timeRange = ftk::Observable<OTIO_NS::TimeRange>::create(OTIO_NS::TimeRange(0.0, 10.0 * 24.0, 24.0));
            p.currentTime = ftk::Observable<OTIO_NS::RationalTime>::create(OTIO_NS::RationalTime(0.0, 24.0));
            p.playback = ftk::Observable<Playback>::create(Playback::Stop);
            p.loop = ftk::Observable<PlaybackLoop>::create(PlaybackLoop::Loop);

            p.playbackTimer = ftk::Timer::create(context);
            p.playbackTimer->setRepeating(true);
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
            FTK_P();
            if (p.timeRange->setIfChanged(value))
            {
                p.currentTime->setIfChanged(value.clamped(p.currentTime->get()));
            }
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
            FTK_P();
            OTIO_NS::RationalTime tmp = value;
            const OTIO_NS::TimeRange& timeRange = p.timeRange->get();
            if (tmp > timeRange.end_time_inclusive())
            {
                tmp = timeRange.start_time();
            }
            else if (tmp < timeRange.start_time())
            {
                tmp = timeRange.end_time_inclusive();
            }
            p.currentTime->setIfChanged(tmp);
        }

        void TimeModel::gotoStart()
        {
            _p->currentTime->setIfChanged(_p->timeRange->get().start_time());
        }

        void TimeModel::gotoEnd()
        {
            _p->currentTime->setIfChanged(_p->timeRange->get().end_time_inclusive());
        }

        void TimeModel::framePrev()
        {
            FTK_P();
            OTIO_NS::RationalTime time = p.currentTime->get();
            time -= OTIO_NS::RationalTime(1.0, time.rate());
            const OTIO_NS::TimeRange& timeRange = p.timeRange->get();
            if (time < timeRange.start_time())
            {
                time = timeRange.end_time_inclusive();
            }
            p.currentTime->setIfChanged(time);
        }

        void TimeModel::frameNext()
        {
            FTK_P();
            OTIO_NS::RationalTime time = p.currentTime->get();
            time += OTIO_NS::RationalTime(1.0, time.rate());
            const OTIO_NS::TimeRange& timeRange = p.timeRange->get();
            if (time > timeRange.end_time_inclusive())
            {
                time = timeRange.start_time();
            }
            p.currentTime->setIfChanged(time);
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
            FTK_P();
            if (p.playback->setIfChanged(value))
            {
                switch (value)
                {
                case Playback::Stop:
                    p.playbackTimer->stop();
                    break;
                case Playback::Forward:
                    p.togglePlayback = value;
                    p.playbackTimer->start(
                        std::chrono::milliseconds(static_cast<int>(1.0 / 24.0 * 1000)),
                        [this]
                        {
                            frameNext();
                        });
                    break;
                case Playback::Reverse:
                    p.togglePlayback = value;
                    p.playbackTimer->start(
                        std::chrono::milliseconds(static_cast<int>(1.0 / 24.0 * 1000)),
                        [this]
                        {
                            framePrev();
                        });
                    break;
                default: break;
                }
            }
        }

        void TimeModel::stop()
        {
            setPlayback(Playback::Stop);
        }

        void TimeModel::forward()
        {
            setPlayback(Playback::Forward);
        }

        void TimeModel::reverse()
        {
            setPlayback(Playback::Reverse);
        }

        void TimeModel::togglePlayback()
        {
            FTK_P();
            setPlayback(Playback::Stop == p.playback->get() ? p.togglePlayback : Playback::Stop);
        }

        PlaybackLoop TimeModel::getLoop() const
        {
            return _p->loop->get();
        }

        std::shared_ptr<ftk::IObservable<PlaybackLoop> > TimeModel::observeLoop() const
        {
            return _p->loop;
        }

        void TimeModel::setLoop(PlaybackLoop value)
        {
            _p->loop->setIfChanged(value);
        }
    }
}
