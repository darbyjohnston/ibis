// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include "TimeActions.h"

#include "App.h"

namespace ibis
{
    void TimeActions::_init(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app)
    {
        auto appWeak = std::weak_ptr<App>(app);
        _actions["Stop"] = ftk::Action::create(
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

        _actions["Forward"] = ftk::Action::create(
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

        _actions["Reverse"] = ftk::Action::create(
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

        _actions["TogglePlayback"] = ftk::Action::create(
            "TogglePlayback",
            ftk::KeyShortcut(ftk::Key::Space),
            [appWeak]
            {
                if (auto document = appWeak.lock()->getDocumentModel()->getCurrent())
                {
                    document->getTimeModel()->togglePlayback();
                }
            });

        _actions["StartFrame"] = ftk::Action::create(
            "Start Frame",
            "FrameStart",
            ftk::KeyShortcut(ftk::Key::Down),
            [appWeak]
            {
                if (auto document = appWeak.lock()->getDocumentModel()->getCurrent())
                {
                    document->getTimeModel()->gotoStart();
                }
            });

        _actions["PrevFrame"] = ftk::Action::create(
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

        _actions["NextFrame"] = ftk::Action::create(
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

        _actions["EndFrame"] = ftk::Action::create(
            "End Frame",
            "FrameEnd",
            ftk::KeyShortcut(ftk::Key::Up),
            [appWeak]
            {
                if (auto document = appWeak.lock()->getDocumentModel()->getCurrent())
                {
                    document->getTimeModel()->gotoEnd();
                }
            });

        _currentObserver = ftk::Observer<std::shared_ptr<models::Document> >::create(
            app->getDocumentModel()->observeCurrent(),
            [this](const std::shared_ptr<models::Document>& value)
            {
                _playbackObserver.reset();
                if (value)
                {
                    _actions["Stop"]->setEnabled(true);
                    _actions["Forward"]->setEnabled(true);
                    _actions["Reverse"]->setEnabled(true);
                    _actions["StartFrame"]->setEnabled(true);
                    _actions["PrevFrame"]->setEnabled(true);
                    _actions["NextFrame"]->setEnabled(true);
                    _actions["EndFrame"]->setEnabled(true);

                    _playbackObserver = ftk::Observer<models::Playback>::create(
                        value->getTimeModel()->observePlayback(),
                        [this](models::Playback value)
                        {
                            _actions["Stop"]->setChecked(models::Playback::Stop == value);
                            _actions["Forward"]->setChecked(models::Playback::Forward == value);
                            _actions["Reverse"]->setChecked(models::Playback::Reverse == value);
                        });
                }
                else
                {
                    _actions["Stop"]->setChecked(true);
                    _actions["Forward"]->setChecked(false);
                    _actions["Reverse"]->setChecked(false);
                    _actions["Stop"]->setEnabled(false);
                    _actions["Forward"]->setEnabled(false);
                    _actions["Reverse"]->setEnabled(false);
                    _actions["StartFrame"]->setEnabled(false);
                    _actions["PrevFrame"]->setEnabled(false);
                    _actions["NextFrame"]->setEnabled(false);
                    _actions["EndFrame"]->setEnabled(false);
                }
            });
    }

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
        return _actions;
    }
}
