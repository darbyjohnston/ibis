// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include "FileActions.h"

#include "App.h"
#include "MainWindow.h"

namespace ibis
{
    void FileActions::_init(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<MainWindow>& mainWindow)
    {
        auto appWeak = std::weak_ptr<App>(app);
        _actions["New"] = ftk::Action::create(
            "New",
            "FileNew",
            ftk::KeyShortcut(ftk::Key::N, ftk::commandKeyModifier),
            [appWeak]
            {
                appWeak.lock()->newDocument();
            });
        _actions["New"]->setTooltip("Create a new file.");

        _actions["Open"] = ftk::Action::create(
            "Open",
            "FileOpen",
            ftk::KeyShortcut(ftk::Key::O, ftk::commandKeyModifier),
            [appWeak]
            {
                appWeak.lock()->open();
            });
        _actions["Open"]->setTooltip("Open a file.");

        _actions["Save"] = ftk::Action::create(
            "Save",
            "FileSave",
            ftk::KeyShortcut(ftk::Key::S, ftk::commandKeyModifier),
            [appWeak]
            {
                appWeak.lock()->save();
            });
        _actions["Save"]->setTooltip("Save the current file.");

        std::weak_ptr<MainWindow> mainWindowWeak(mainWindow);
        _actions["Close"] = ftk::Action::create(
            "Close",
            "FileClose",
            ftk::KeyShortcut(ftk::Key::E, ftk::commandKeyModifier),
            [appWeak, mainWindowWeak]
            {
                appWeak.lock()->getDocumentModel()->close(
                    mainWindowWeak.lock()->getCurrentTab());
            });
        _actions["Close"]->setTooltip("Close the current file.");

        _actions["CloseAll"] = ftk::Action::create(
            "Close All",
            "FileCloseAll",
            ftk::KeyShortcut(ftk::Key::E, ftk::commandKeyModifier, ftk::KeyModifier::Shift),
            [appWeak]
            {
                appWeak.lock()->getDocumentModel()->closeAll();
            });
        _actions["CloseAll"]->setTooltip("Close all files.");

        _actions["Exit"] = ftk::Action::create(
            "Exit",
            ftk::KeyShortcut(ftk::Key::Q, ftk::commandKeyModifier),
            [appWeak]
            {
                if (auto app = appWeak.lock())
                {
                    app->exit();
                }
            });

        _documentsObserver = ftk::ListObserver<std::shared_ptr<models::Document> >::create(
            app->getDocumentModel()->observe(),
            [this, appWeak](const std::vector<std::shared_ptr<models::Document> >& value)
            {
                _actions["Close"]->setEnabled(!value.empty());
                _actions["CloseAll"]->setEnabled(!value.empty());
            });
    }

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
        return _actions;
    }
}