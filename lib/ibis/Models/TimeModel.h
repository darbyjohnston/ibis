// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#pragma once

#include <ibis/Core/Time.h>

#include <ftk/Core/Observable.h>

namespace ftk
{
    class Context;
}

namespace ibis
{
    namespace models
    {
        //! Playback mode.
        enum class Playback
        {
            Stop,
            Forward,
            Reverse,

            Count,
            First = Stop
        };
        FTK_ENUM(Playback);

        //! Playback loop mode.
        enum class PlaybackLoop
        {
            Loop,
            Once,
            PingPong,

            Count,
            First = Loop
        };
        FTK_ENUM(PlaybackLoop);

        //! Time model.
        class TimeModel : public std::enable_shared_from_this<TimeModel>
        {
            FTK_NON_COPYABLE(TimeModel);

        protected:
            void _init(const std::shared_ptr<ftk::Context>&);

            TimeModel();

        public:
            virtual ~TimeModel();

            //! Create a new model.
            static std::shared_ptr<TimeModel> create(
                const std::shared_ptr<ftk::Context>&);

            //! \name Time
            ///@{

            const OTIO_NS::TimeRange& getTimeRange();
            std::shared_ptr<ftk::IObservable<OTIO_NS::TimeRange> > observeTimeRange() const;
            void setTimeRange(const OTIO_NS::TimeRange&);

            const OTIO_NS::RationalTime& getCurrentTime();
            std::shared_ptr<ftk::IObservable<OTIO_NS::RationalTime> > observeCurrentTime() const;
            void setCurrentTime(const OTIO_NS::RationalTime&);

            void gotoStart();
            void gotoEnd();
            void framePrev();
            void frameNext();

            ///@}

            //! \name Playback
            ///@{

            Playback getPlayback() const;
            std::shared_ptr<ftk::IObservable<Playback> > observePlayback() const;
            void setPlayback(Playback);

            void stop();
            void forward();
            void reverse();
            void togglePlayback();

            PlaybackLoop getLoop() const;
            std::shared_ptr<ftk::IObservable<PlaybackLoop> > observeLoop() const;
            void setLoop(PlaybackLoop);

            ///@}

        private:
            FTK_PRIVATE();
        };
    }
}
