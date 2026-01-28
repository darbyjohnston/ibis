// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include "ViewActions.h"

#include "App.h"
#include "DocumentWidget.h"
#include "MainWindow.h"

#include <ibis/ui/Viewport.h>

#include <ibis/Models/DocumentModel.h>

namespace ibis
{
    struct ViewActions::Private
    {
        std::map<std::string, std::shared_ptr<ftk::Action> > actions;

        std::shared_ptr<ftk::Observer<std::shared_ptr<DocumentWidget> > > documentWidgetObserver;
        std::shared_ptr<ftk::Observer<bool> > frameObserver;
    };

    void ViewActions::_init(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<MainWindow>& mainWindow)
    {
        FTK_P();

        auto mainWindowWeak = std::weak_ptr<MainWindow>(mainWindow);
        p.actions["Frame"] = ftk::Action::create(
            "Frame",
            "ViewFrame",
            ftk::Key::Backspace,
            [mainWindowWeak](bool value)
            {
                if (auto mainWindow = mainWindowWeak.lock())
                {
                    if (auto documentWidget = mainWindow->getDocumentWidget())
                    {
                        documentWidget->getViewport()->setFrameView(value);
                    }
                }
            });
        p.actions["Frame"]->setTooltip("Toggle whether to automatically frame the view.");

        p.actions["ZoomReset"] = ftk::Action::create(
            "Zoom Reset",
            "ViewZoomReset",
            ftk::Key::_0,
            [mainWindowWeak]
            {
                if (auto mainWindow = mainWindowWeak.lock())
                {
                    if (auto documentWidget = mainWindow->getDocumentWidget())
                    {
                        documentWidget->getViewport()->viewZoomReset();
                    }
                }
            });
        p.actions["ZoomReset"]->setTooltip("Reset the view zoom.");

        p.actions["ZoomIn"] = ftk::Action::create(
            "Zoom In",
            "ViewZoomIn",
            ftk::Key::Equals,
            [mainWindowWeak]
            {
                if (auto mainWindow = mainWindowWeak.lock())
                {
                    if (auto documentWidget = mainWindow->getDocumentWidget())
                    {
                        documentWidget->getViewport()->viewZoomIn();
                    }
                }
            });
        p.actions["ZoomIn"]->setTooltip("Zoom the view in.");

        p.actions["ZoomOut"] = ftk::Action::create(
            "Zoom Out",
            "ViewZoomOut",
            ftk::Key::Minus,
            [mainWindowWeak]
            {
                if (auto mainWindow = mainWindowWeak.lock())
                {
                    if (auto documentWidget = mainWindow->getDocumentWidget())
                    {
                        documentWidget->getViewport()->viewZoomOut();
                    }
                }
            });
        p.actions["ZoomOut"]->setTooltip("Zoom the view out.");

        p.documentWidgetObserver = ftk::Observer<std::shared_ptr<DocumentWidget> >::create(
            mainWindow->observeDocumentWidget(),
            [this](const std::shared_ptr<DocumentWidget>& value)
            {
                FTK_P();
                if (value)
                {
                    for (const auto& i : p.actions)
                    {
                        i.second->setEnabled(true);
                    }

                    p.frameObserver = ftk::Observer<bool>::create(
                        value->getViewport()->observeFrameView(),
                        [this](bool value)
                        {
                            _p->actions["Frame"]->setChecked(value);
                        });
                }
                else
                {
                    for (const auto& i : p.actions)
                    {
                        i.second->setEnabled(false);
                    }

                    p.frameObserver.reset();
                }
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