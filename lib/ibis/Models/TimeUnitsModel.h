// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#pragma once

#include <ibis/Core/Time.h>

#include <ftk/Core/Observable.h>

namespace ftk
{
    class Context;
    class Settings;
}

namespace ibis
{
    namespace models
    {
        //! Time units.
        enum class TimeUnits
        {
            Frames,
            Seconds,
            Timecode,

            Count,
            First = Frames
        };
        FTK_ENUM(TimeUnits);

        //! Convert a time value to text.
        std::string timeToText(const OTIO_NS::RationalTime&, TimeUnits);

        //! Convert text to a time value.
        OTIO_NS::RationalTime textToTime(
            const std::string& text,
            double                 rate,
            TimeUnits              units,
            opentime::ErrorStatus* error = nullptr);

        //! Get a time units format string.
        std::string formatString(TimeUnits);

        //! Get a time units validator regular expression.
        std::string validator(TimeUnits);

        //! Time units model.
        class TimeUnitsModel : public std::enable_shared_from_this<TimeUnitsModel>
        {
            FTK_NON_COPYABLE(TimeUnitsModel);

        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ftk::Settings>&);

            TimeUnitsModel();

        public:
            virtual ~TimeUnitsModel();

            //! Create a new model.
            static std::shared_ptr<TimeUnitsModel> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ftk::Settings>&);

            //! Get the time units.
            TimeUnits getTimeUnits() const;

            //! Observe the time units.
            std::shared_ptr<ftk::IObservable<TimeUnits> > observeTimeUnits() const;

            //! Set the time units.
            void setTimeUnits(TimeUnits);

            //! Get a time label in the current time units.
            std::string getLabel(const OTIO_NS::RationalTime&) const;

        private:
            FTK_PRIVATE();
        };
    }
}
