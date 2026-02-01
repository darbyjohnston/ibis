// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include "WindowToolBar.h"

#include "App.h"
#include "WindowActions.h"

namespace ibis
{
    struct WindowToolBar::Private
    {
    };

    void WindowToolBar::_init(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<WindowActions>& windowActions,
        const std::shared_ptr<IWidget>& parent)
    {
        ftk::ToolBar::_init(context, ftk::Orientation::Horizontal, parent);
        FTK_P();

        auto actions = windowActions->getActions();
        addAction(actions["FullScreen"]);
        addAction(actions["SidePanel"]);
    }

    WindowToolBar::WindowToolBar() :
        _p(new Private)
    {}

    WindowToolBar::~WindowToolBar()
    {}

    std::shared_ptr<WindowToolBar> WindowToolBar::create(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<WindowActions>& windowActions,
        const std::shared_ptr<IWidget>& parent)
    {
        auto out = std::shared_ptr<WindowToolBar>(new WindowToolBar);
        out->_init(context, app, windowActions, parent);
        return out;
    }
}
