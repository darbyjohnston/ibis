// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include "TimeMenu.h"

#include "App.h"
#include "TimeActions.h"

namespace ibis
{
    struct TimeMenu::Private
    {
        std::shared_ptr<ftk::Menu> loopMenu;
    };

    void TimeMenu::_init(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<TimeActions>& timeActions,
        const std::shared_ptr<IWidget>& parent)
    {
        ftk::Menu::_init(context, parent);
        FTK_P();

        auto actions = timeActions->getActions();
        addAction(actions["Stop"]);
        addAction(actions["Forward"]);
        addAction(actions["Reverse"]);
        addAction(actions["TogglePlayback"]);
        p.loopMenu = addSubMenu("Loop");
        p.loopMenu->addAction(actions["PlaybackLoop"]);
        p.loopMenu->addAction(actions["PlaybackOnce"]);
        p.loopMenu->addAction(actions["PlaybackPingPong"]);
        addDivider();
        addAction(actions["StartFrame"]);
        addAction(actions["PrevFrame"]);
        addAction(actions["NextFrame"]);
        addAction(actions["EndFrame"]);
    }

    TimeMenu::TimeMenu() :
        _p(new Private)
    {}

    TimeMenu::~TimeMenu()
    {}

    std::shared_ptr<TimeMenu> TimeMenu::create(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<TimeActions>& timeActions,
        const std::shared_ptr<IWidget>& parent)
    {
        auto out = std::shared_ptr<TimeMenu>(new TimeMenu);
        out->_init(context, app, timeActions, parent);
        return out;
    }
}