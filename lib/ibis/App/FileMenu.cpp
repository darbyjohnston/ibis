// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include "FileMenu.h"

#include "App.h"
#include "FileActions.h"

namespace ibis
{
    struct FileMenu::Private
    {
        std::vector<std::shared_ptr<ftk::Action> > recentActions;
        std::shared_ptr<ftk::Menu> recentMenu;
    };

    void FileMenu::_init(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<FileActions>& fileActions,
        const std::shared_ptr<IWidget>& parent)
    {
        ftk::Menu::_init(context, parent);
        FTK_P();

        auto actions = fileActions->getActions();
        addAction(actions["New"]);
        addAction(actions["Open"]);
        addAction(actions["Save"]);
        addAction(actions["Close"]);
        addAction(actions["CloseAll"]);
        p.recentMenu = addSubMenu("Recent");
        addDivider();
        addAction(actions["Render"]);
        addDivider();
        addAction(actions["Exit"]);
    }

    FileMenu::FileMenu() :
        _p(new Private)
    {}

    FileMenu::~FileMenu()
    {}

    std::shared_ptr<FileMenu> FileMenu::create(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<FileActions>& fileActions,
        const std::shared_ptr<IWidget>& parent)
    {
        auto out = std::shared_ptr<FileMenu>(new FileMenu);
        out->_init(context, app, fileActions, parent);
        return out;
    }
}