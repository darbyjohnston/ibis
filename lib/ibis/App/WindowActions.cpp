// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include "WindowActions.h"

#include "App.h"
#include "MainWindow.h"

namespace ibis
{
    struct WindowActions::Private
    {
        std::map<std::string, std::shared_ptr<ftk::Action> > actions;

        std::shared_ptr<ftk::Observer<bool> > fullScreenObserver;
        std::shared_ptr<ftk::Observer<std::pair<SidePanel, bool> > > sidePanelObserver;
    };

    void WindowActions::_init(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<MainWindow>& mainWindow)
    {
        FTK_P();

        auto mainWindowWeak = std::weak_ptr<MainWindow>(mainWindow);
        p.actions["FullScreen"] = ftk::Action::create(
            "Full Screen",
            "WindowFullScreen",
            ftk::KeyShortcut(ftk::Key::U, static_cast<int>(ftk::commandKeyModifier)),
            [mainWindowWeak](bool value)
            {
                if (auto mainWindow = mainWindowWeak.lock())
                {
                    mainWindow->setFullScreen(value);
                }
            });
        p.actions["FullScreen"]->setTooltip("Toggle the window full screen mode.");

        const std::vector<ftk::Key> sidePanelShortcuts =
        {
            ftk::Key::F1,
            ftk::Key::F2,
            ftk::Key::F3,
            ftk::Key::F4,
            ftk::Key::F5
        };
        const std::vector<std::string> sidePanelTooltips =
        {
            "Toggle the node browser.",
            "Toggle the node editor.",
            "Toggle the settings.",
            "Toggle the diagnostics.",
            "Toggle the system log."
        };
        const auto sidePanelLabels = getSidePanelLabels();
        for (size_t i = 0; i < sidePanelLabels.size(); ++i)
        {
            p.actions[sidePanelLabels[i]] = ftk::Action::create(
                sidePanelLabels[i],
                ftk::KeyShortcut(sidePanelShortcuts[i]),
                [mainWindowWeak, i]
                {
                    if (auto mainWindow = mainWindowWeak.lock())
                    {
                        mainWindow->setSidePanel(static_cast<SidePanel>(i));
                    }
                });
            p.actions[sidePanelLabels[i]]->setTooltip(sidePanelTooltips[i]);
        }

        p.fullScreenObserver = ftk::Observer<bool>::create(
            mainWindow->observeFullScreen(),
            [this](bool value)
            {
                _p->actions["FullScreen"]->setChecked(value);
            });

        p.sidePanelObserver = ftk::Observer<std::pair<SidePanel, bool> >::create(
            mainWindow->observeSidePanel(),
            [this](const std::pair<SidePanel, bool>& value)
            {
                FTK_P();
                for (const auto i : getSidePanelEnums())
                {
                    const std::string label = getLabel(i);
                    p.actions[label]->setChecked(value.first == i && value.second);
                }
            });
    }

    WindowActions::WindowActions() :
        _p(new Private)
    {}

    WindowActions::~WindowActions()
    {}

    std::shared_ptr<WindowActions> WindowActions::create(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<MainWindow>& mainWindow)
    {
        auto out = std::shared_ptr<WindowActions>(new WindowActions);
        out->_init(context, app, mainWindow);
        return out;
    }

    const std::map<std::string, std::shared_ptr<ftk::Action> >& WindowActions::getActions() const
    {
        return _p->actions;
    }
}