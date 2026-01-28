// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include "ViewToolBar.h"

#include "App.h"
#include "ViewActions.h"

namespace ibis
{
    struct ViewToolBar::Private
    {
    };

    void ViewToolBar::_init(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<ViewActions>& viewActions,
        const std::shared_ptr<IWidget>& parent)
    {
        ftk::ToolBar::_init(context, ftk::Orientation::Horizontal, parent);
        FTK_P();

        auto actions = viewActions->getActions();
        addAction(actions["Frame"]);
        addAction(actions["ZoomReset"]);
        addAction(actions["ZoomIn"]);
        addAction(actions["ZoomOut"]);
    }

    ViewToolBar::ViewToolBar() :
        _p(new Private)
    {}

    ViewToolBar::~ViewToolBar()
    {}

    std::shared_ptr<ViewToolBar> ViewToolBar::create(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<ViewActions>& viewActions,
        const std::shared_ptr<IWidget>& parent)
    {
        auto out = std::shared_ptr<ViewToolBar>(new ViewToolBar);
        out->_init(context, app, viewActions, parent);
        return out;
    }
}
