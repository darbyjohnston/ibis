// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include "FileActions.h"

#include "App.h"
#include "MainWindow.h"

#include <ibis/Models/DocumentModel.h>

namespace ibis
{
    struct FileActions::Private
    {
        std::map<std::string, std::shared_ptr<ftk::Action> > actions;

        std::shared_ptr<ftk::ListObserver<std::shared_ptr<models::Document> > > documentsObserver;
    };

    void FileActions::_init(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<MainWindow>& mainWindow)
    {
        FTK_P();

        auto appWeak = std::weak_ptr<App>(app);
        p.actions["New"] = ftk::Action::create(
            "New",
            "FileNew",
            ftk::KeyShortcut(ftk::Key::N, ftk::commandKeyModifier),
            [appWeak]
            {
                appWeak.lock()->newDocument();
            });
        p.actions["New"]->setTooltip("Create a new file.");

        p.actions["Open"] = ftk::Action::create(
            "Open",
            "FileOpen",
            ftk::KeyShortcut(ftk::Key::O, ftk::commandKeyModifier),
            [appWeak]
            {
                appWeak.lock()->open();
            });
        p.actions["Open"]->setTooltip("Open a file.");

        p.actions["Save"] = ftk::Action::create(
            "Save",
            "FileSave",
            ftk::KeyShortcut(ftk::Key::S, ftk::commandKeyModifier),
            [appWeak]
            {
                appWeak.lock()->save();
            });
        p.actions["Save"]->setTooltip("Save the current file.");

        std::weak_ptr<MainWindow> mainWindowWeak(mainWindow);
        p.actions["Close"] = ftk::Action::create(
            "Close",
            "FileClose",
            ftk::KeyShortcut(ftk::Key::E, ftk::commandKeyModifier),
            [appWeak, mainWindowWeak]
            {
                appWeak.lock()->getDocumentModel()->close(
                    mainWindowWeak.lock()->getCurrentTab());
            });
        p.actions["Close"]->setTooltip("Close the current file.");

        p.actions["CloseAll"] = ftk::Action::create(
            "Close All",
            "FileCloseAll",
            ftk::KeyShortcut(ftk::Key::E, ftk::commandKeyModifier, ftk::KeyModifier::Shift),
            [appWeak]
            {
                appWeak.lock()->getDocumentModel()->closeAll();
            });
        p.actions["CloseAll"]->setTooltip("Close all files.");

        p.actions["Exit"] = ftk::Action::create(
            "Exit",
            ftk::KeyShortcut(ftk::Key::Q, ftk::commandKeyModifier),
            [appWeak]
            {
                if (auto app = appWeak.lock())
                {
                    app->exit();
                }
            });

        p.documentsObserver = ftk::ListObserver<std::shared_ptr<models::Document> >::create(
            app->getDocumentModel()->observe(),
            [this, appWeak](const std::vector<std::shared_ptr<models::Document> >& value)
            {
                FTK_P();
                p.actions["Save"]->setEnabled(!value.empty());
                p.actions["Close"]->setEnabled(!value.empty());
                p.actions["CloseAll"]->setEnabled(!value.empty());
            });
    }

    FileActions::FileActions() :
        _p(new Private)
    {}

    FileActions::~FileActions()
    {}

    std::shared_ptr<FileActions> FileActions::create(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<MainWindow>& mainWindow)
    {
        auto out = std::shared_ptr<FileActions>(new FileActions);
        out->_init(context, app, mainWindow);
        return out;
    }

    const std::map<std::string, std::shared_ptr<ftk::Action> >& FileActions::getActions() const
    {
        return _p->actions;
    }
}