// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include "EditToolBar.h"

#include "App.h"
#include "EditActions.h"

namespace ibis
{
    struct EditToolBar::Private
    {
    };

    void EditToolBar::_init(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<EditActions>& editActions,
        const std::shared_ptr<IWidget>& parent)
    {
        ftk::ToolBar::_init(context, ftk::Orientation::Horizontal, parent);
        FTK_P();

        auto actions = editActions->getActions();
        addAction(actions["Undo"]);
        addAction(actions["Redo"]);
    }

    EditToolBar::EditToolBar() :
        _p(new Private)
    {}

    EditToolBar::~EditToolBar()
    {}

    std::shared_ptr<EditToolBar> EditToolBar::create(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<EditActions>& editActions,
        const std::shared_ptr<IWidget>& parent)
    {
        auto out = std::shared_ptr<EditToolBar>(new EditToolBar);
        out->_init(context, app, editActions, parent);
        return out;
    }
}
