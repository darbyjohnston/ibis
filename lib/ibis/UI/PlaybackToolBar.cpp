// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include <ibis/UI/PlaybackToolBar.h>

#include <ibis/UI/PlaybackLoopWidget.h>

#include <ftk/UI/Action.h>

namespace ibis
{
    namespace ui
    {
        struct PlaybackToolBar::Private
        {
            std::map<std::string, std::shared_ptr<ftk::Action> > actions;
            std::shared_ptr<PlaybackLoopWidget> loopWidget;
            std::shared_ptr<ftk::Observer<models::Playback> > playbackObserver;
            std::shared_ptr<ftk::Observer<models::PlaybackLoop> > loopObserver;
        };

        void PlaybackToolBar::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::TimeModel>& timeModel,
            const std::shared_ptr<IWidget>& parent)
        {
            ToolBar::_init(context, ftk::Orientation::Horizontal, parent);
            FTK_P();

            p.actions["Stop"] = ftk::Action::create(
                "Stop",
                "PlaybackStop",
                ftk::Key::K,
                [timeModel]
                {
                    timeModel->stop();
                });
            p.actions["Stop"]->setTooltip("Stop playback.");

            p.actions["Forward"] = ftk::Action::create(
                "Forward",
                "PlaybackForward",
                ftk::Key::L,
                [timeModel]
                {
                    timeModel->forward();
                });
            p.actions["Forward"]->setTooltip("Start forward playback.");

            p.actions["Reverse"] = ftk::Action::create(
                "Reverse",
                "PlaybackReverse",
                ftk::Key::J,
                [timeModel]
                {
                    timeModel->reverse();
                });
            p.actions["Reverse"]->setTooltip("Start reverse playback.");

            p.loopWidget = PlaybackLoopWidget::create(context);
            p.loopWidget->setTooltip("Playback loop mode.");

            addAction(p.actions["Reverse"]);
            addAction(p.actions["Stop"]);
            addAction(p.actions["Forward"]);
            addWidget(p.loopWidget);

            p.loopWidget->setCallback(
                [timeModel](models::PlaybackLoop value)
                {
                    timeModel->setLoop(value);
                });

            p.playbackObserver = ftk::Observer<models::Playback>::create(
                timeModel->observePlayback(),
                [this](models::Playback value)
                {
                    _p->actions["Stop"]->setChecked(models::Playback::Stop == value);
                    _p->actions["Forward"]->setChecked(models::Playback::Forward == value);
                    _p->actions["Reverse"]->setChecked(models::Playback::Reverse == value);
                });
            
            p.loopObserver = ftk::Observer<models::PlaybackLoop>::create(
                timeModel->observeLoop(),
                [this](models::PlaybackLoop value)
                {
                    _p->loopWidget->setLoop(value);
                });
        }

        PlaybackToolBar::PlaybackToolBar() :
            _p(new Private)
        {}

        PlaybackToolBar::~PlaybackToolBar()
        {}

        std::shared_ptr<PlaybackToolBar> PlaybackToolBar::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::TimeModel>& timeModel,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<PlaybackToolBar>(new PlaybackToolBar);
            out->_init(context, timeModel, parent);
            return out;
        }
            
        const std::map<std::string, std::shared_ptr<ftk::Action> >& PlaybackToolBar::getActions() const
        {
            return _p->actions;
        }
    }
}
