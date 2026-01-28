// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include "TimeActions.h"

#include "App.h"

#include <ibis/Models/DocumentModel.h>
#include <ibis/Models/TimeModel.h>

namespace ibis
{
    struct TimeActions::Private
    {
        std::map<std::string, std::shared_ptr<ftk::Action> > actions;

        std::shared_ptr<ftk::Observer<std::shared_ptr<models::Document> > > currentObserver;
        std::shared_ptr<ftk::Observer<models::Playback> > playbackObserver;
        std::shared_ptr<ftk::Observer<models::PlaybackLoop> > loopObserver;
    };

    void TimeActions::_init(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app)
    {
        FTK_P();

        auto appWeak = std::weak_ptr<App>(app);
        p.actions["Stop"] = ftk::Action::create(
            "Stop",
            "PlaybackStop",
            ftk::KeyShortcut(ftk::Key::K),
            [appWeak]
            {
                if (auto document = appWeak.lock()->getDocumentModel()->getCurrent())
                {
                    document->getTimeModel()->stop();
                }
            });

        p.actions["Forward"] = ftk::Action::create(
            "Forward",
            "PlaybackForward",
            ftk::KeyShortcut(ftk::Key::L),
            [appWeak]
            {
                if (auto document = appWeak.lock()->getDocumentModel()->getCurrent())
                {
                    document->getTimeModel()->forward();
                }
            });

        p.actions["Reverse"] = ftk::Action::create(
            "Reverse",
            "PlaybackReverse",
            ftk::KeyShortcut(ftk::Key::J),
            [appWeak]
            {
                if (auto document = appWeak.lock()->getDocumentModel()->getCurrent())
                {
                    document->getTimeModel()->reverse();
                }
            });

        p.actions["TogglePlayback"] = ftk::Action::create(
            "Toggle Playback",
            ftk::KeyShortcut(ftk::Key::Space),
            [appWeak]
            {
                if (auto document = appWeak.lock()->getDocumentModel()->getCurrent())
                {
                    document->getTimeModel()->togglePlayback();
                }
            });

        const auto& loopLabels = models::getPlaybackLoopLabels();
        for (const auto& loop : models::getPlaybackLoopEnums())
        {
            const std::string label = loopLabels[static_cast<size_t>(loop)];
            p.actions["Playback" + label] = ftk::Action::create(
                label,
                [appWeak, loop]
                {
                    if (auto document = appWeak.lock()->getDocumentModel()->getCurrent())
                    {
                        document->getTimeModel()->setLoop(loop);
                    }
                });
        }

        p.actions["StartFrame"] = ftk::Action::create(
            "Start Frame",
            "FrameStart",
            ftk::KeyShortcut(ftk::Key::Up),
            [appWeak]
            {
                if (auto document = appWeak.lock()->getDocumentModel()->getCurrent())
                {
                    document->getTimeModel()->gotoStart();
                }
            });

        p.actions["PrevFrame"] = ftk::Action::create(
            "Previous Frame",
            "FramePrev",
            ftk::KeyShortcut(ftk::Key::Left),
            [appWeak]
            {
                if (auto document = appWeak.lock()->getDocumentModel()->getCurrent())
                {
                    document->getTimeModel()->framePrev();
                }
            });

        p.actions["NextFrame"] = ftk::Action::create(
            "Next Frame",
            "FrameNext",
            ftk::KeyShortcut(ftk::Key::Right),
            [appWeak]
            {
                if (auto document = appWeak.lock()->getDocumentModel()->getCurrent())
                {
                    document->getTimeModel()->frameNext();
                }
            });

        p.actions["EndFrame"] = ftk::Action::create(
            "End Frame",
            "FrameEnd",
            ftk::KeyShortcut(ftk::Key::Down),
            [appWeak]
            {
                if (auto document = appWeak.lock()->getDocumentModel()->getCurrent())
                {
                    document->getTimeModel()->gotoEnd();
                }
            });

        p.currentObserver = ftk::Observer<std::shared_ptr<models::Document> >::create(
            app->getDocumentModel()->observeCurrent(),
            [this](const std::shared_ptr<models::Document>& value)
            {
                FTK_P();
                p.playbackObserver.reset();
                p.loopObserver.reset();
                if (value)
                {
                    for (const auto& i : p.actions)
                    {
                        i.second->setEnabled(true);
                    }

                    p.playbackObserver = ftk::Observer<models::Playback>::create(
                        value->getTimeModel()->observePlayback(),
                        [this](models::Playback value)
                        {
                            FTK_P();
                            p.actions["Stop"]->setChecked(models::Playback::Stop == value);
                            p.actions["Forward"]->setChecked(models::Playback::Forward == value);
                            p.actions["Reverse"]->setChecked(models::Playback::Reverse == value);
                        });

                    p.loopObserver = ftk::Observer<models::PlaybackLoop>::create(
                        value->getTimeModel()->observeLoop(),
                        [this](models::PlaybackLoop value)
                        {
                            FTK_P();
                            p.actions["PlaybackLoop"]->setChecked(models::PlaybackLoop::Loop == value);
                            p.actions["PlaybackOnce"]->setChecked(models::PlaybackLoop::Once == value);
                            p.actions["PlaybackPingPong"]->setChecked(models::PlaybackLoop::PingPong == value);
                        });
                }
                else
                {
                    p.actions["Stop"]->setChecked(true);
                    p.actions["Forward"]->setChecked(false);
                    p.actions["Reverse"]->setChecked(false);

                    p.actions["PlaybackLoop"]->setChecked(true);
                    p.actions["PlaybackOnce"]->setChecked(false);
                    p.actions["PlaybackPingPong"]->setChecked(false);

                    for (const auto& i : p.actions)
                    {
                        i.second->setEnabled(false);
                    }
                }
            });
    }

    TimeActions::TimeActions() :
        _p(new Private)
    {}

    TimeActions::~TimeActions()
    {}

    std::shared_ptr<TimeActions> TimeActions::create(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app)
    {
        auto out = std::shared_ptr<TimeActions>(new TimeActions);
        out->_init(context, app);
        return out;
    }

    const std::map<std::string, std::shared_ptr<ftk::Action> >& TimeActions::getActions() const
    {
        return _p->actions;
    }
}
