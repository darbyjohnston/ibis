// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include "EditMenu.h"

#include "App.h"
#include "EditActions.h"

namespace ibis
{
    void EditMenu::_init(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<EditActions>& editActions,
        const std::shared_ptr<IWidget>& parent)
    {
        ftk::Menu::_init(context, parent);

        auto actions = editActions->getActions();
        addAction(actions["Undo"]);
        addAction(actions["Redo"]);
    }

    EditMenu::~EditMenu()
    {}

    std::shared_ptr<EditMenu> EditMenu::create(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<EditActions>& editActions,
        const std::shared_ptr<IWidget>& parent)
    {
        auto out = std::shared_ptr<EditMenu>(new EditMenu);
        out->_init(context, app, editActions, parent);
        return out;
    }
}