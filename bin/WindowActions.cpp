// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include "WindowActions.h"

#include "App.h"
#include "MainWindow.h"

namespace ibis
{
    void WindowActions::_init(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<MainWindow>& mainWindow)
    {
        auto appWeak = std::weak_ptr<App>(app);

        _currentObserver = ftk::Observer<std::shared_ptr<models::Document> >::create(
            app->getDocumentModel()->observeCurrent(),
            [this](const std::shared_ptr<models::Document>& value)
            {
            });
    }

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
        return _actions;
    }
}