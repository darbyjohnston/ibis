// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#pragma once

#include <ibis/Models/Document.h>
#include <ibis/Models/TimeModel.h>

#include <ftk/UI/Action.h>

namespace ibis
{
    class App;

    //! Time actions.
    class TimeActions : public std::enable_shared_from_this<TimeActions>
    {
        FTK_NON_COPYABLE(TimeActions);

    protected:
        void _init(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&);

        TimeActions() = default;

    public:
        ~TimeActions();

        static std::shared_ptr<TimeActions> create(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&);

        const std::map<std::string, std::shared_ptr<ftk::Action> >& getActions() const;

    private:
        std::map<std::string, std::shared_ptr<ftk::Action> > _actions;

        std::shared_ptr<ftk::Observer<std::shared_ptr<models::Document> > > _currentObserver;
        std::shared_ptr<ftk::Observer<models::Playback> > _playbackObserver;
    };
}
