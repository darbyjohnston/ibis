// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ftk/Core/Context.h>
#include <ftk/Core/Observable.h>

#include <opentimelineio/version.h>

#include <filesystem>

namespace ibis
{
    namespace models
    {
        //! Document.
        class Document : public std::enable_shared_from_this<Document>
        {
        protected:
            void _init(const std::shared_ptr<ftk::Context>&);

            Document();

        public:
            virtual ~Document();

            //! Create a new document.
            static std::shared_ptr<Document> create(
                const std::shared_ptr<ftk::Context>&);

            //! Create a new document.
            static std::shared_ptr<Document> create(
                const std::shared_ptr<ftk::Context>&,
                const std::filesystem::path&);

            //! \name Path
            ///@{

            const std::filesystem::path& getPath();
            std::shared_ptr<ftk::IObservable<std::filesystem::path> > observePath() const;

            ///@}

            //! \name Time Range
            ///@{

            const OTIO_NS::TimeRange& getTimeRange();
            std::shared_ptr<ftk::IObservable<OTIO_NS::TimeRange> > observeTimeRange() const;
            void setTimeRange(const OTIO_NS::TimeRange&);

            ///@}

        private:
            FTK_PRIVATE();
        };
    }
}
