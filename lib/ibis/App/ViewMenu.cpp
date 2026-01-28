// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include "ViewMenu.h"

#include "App.h"
#include "ViewActions.h"

namespace ibis
{
    void ViewMenu::_init(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<ViewActions>& viewActions,
        const std::shared_ptr<IWidget>& parent)
    {
        ftk::Menu::_init(context, parent);

        auto actions = viewActions->getActions();
        addAction(actions["Frame"]);
        addAction(actions["ZoomReset"]);
        addAction(actions["ZoomIn"]);
        addAction(actions["ZoomOut"]);
    }

    ViewMenu::~ViewMenu()
    {}

    std::shared_ptr<ViewMenu> ViewMenu::create(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<ViewActions>& viewActions,
        const std::shared_ptr<IWidget>& parent)
    {
        auto out = std::shared_ptr<ViewMenu>(new ViewMenu);
        out->_init(context, app, viewActions, parent);
        return out;
    }
}