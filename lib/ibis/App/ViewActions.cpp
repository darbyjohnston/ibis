// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include "ViewActions.h"

#include "App.h"
#include "MainWindow.h"

#include <ibis/Models/DocumentModel.h>

namespace ibis
{
    struct ViewActions::Private
    {
        std::map<std::string, std::shared_ptr<ftk::Action> > actions;

        std::shared_ptr<ftk::Observer<std::shared_ptr<models::Document> > > currentObserver;
    };

    void ViewActions::_init(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<MainWindow>& mainWindow)
    {
        FTK_P();

        auto appWeak = std::weak_ptr<App>(app);

        p.currentObserver = ftk::Observer<std::shared_ptr<models::Document> >::create(
            app->getDocumentModel()->observeCurrent(),
            [this](const std::shared_ptr<models::Document>& value)
            {
            });
    }

    ViewActions::ViewActions() :
        _p(new Private)
    {}

    ViewActions::~ViewActions()
    {}

    std::shared_ptr<ViewActions> ViewActions::create(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<MainWindow>& mainWindow)
    {
        auto out = std::shared_ptr<ViewActions>(new ViewActions);
        out->_init(context, app, mainWindow);
        return out;
    }

    const std::map<std::string, std::shared_ptr<ftk::Action> >& ViewActions::getActions() const
    {
        return _p->actions;
    }
}