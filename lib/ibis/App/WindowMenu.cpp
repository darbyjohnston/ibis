// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include "WindowMenu.h"

#include "App.h"
#include "SidePanel.h"
#include "WindowActions.h"

namespace ibis
{
    struct WindowMenu::Private
    {
        std::shared_ptr<ftk::Menu> resizeMenu;
    };

    void WindowMenu::_init(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<WindowActions>& windowActions,
        const std::shared_ptr<IWidget>& parent)
    {
        ftk::Menu::_init(context, parent);
        FTK_P();
        auto actions = windowActions->getActions();
        addAction(actions["FullScreen"]);
        addDivider();
        p.resizeMenu = addSubMenu("Resize");
        addDivider();
        for (const auto& i : getSidePanelLabels())
        {
            addAction(actions[i]);
        }
    }

    WindowMenu::WindowMenu() :
        _p(new Private)
    {}

    WindowMenu::~WindowMenu()
    {}

    std::shared_ptr<WindowMenu> WindowMenu::create(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<WindowActions>& windowActions,
        const std::shared_ptr<IWidget>& parent)
    {
        auto out = std::shared_ptr<WindowMenu>(new WindowMenu);
        out->_init(context, app, windowActions, parent);
        return out;
    }
}