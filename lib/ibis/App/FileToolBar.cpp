// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include "FileToolBar.h"

#include "App.h"
#include "FileActions.h"

namespace ibis
{
    struct FileToolBar::Private
    {
    };

    void FileToolBar::_init(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<FileActions>& fileActions,
        const std::shared_ptr<IWidget>& parent)
    {
        ftk::ToolBar::_init(context, ftk::Orientation::Horizontal, parent);
        FTK_P();

        auto actions = fileActions->getActions();
        addAction(actions["New"]);
        addAction(actions["Open"]);
        addAction(actions["Save"]);
        addAction(actions["Close"]);
        addAction(actions["CloseAll"]);
    }

    FileToolBar::FileToolBar() :
        _p(new Private)
    {}

    FileToolBar::~FileToolBar()
    {}

    std::shared_ptr<FileToolBar> FileToolBar::create(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<FileActions>& fileActions,
        const std::shared_ptr<IWidget>& parent)
    {
        auto out = std::shared_ptr<FileToolBar>(new FileToolBar);
        out->_init(context, app, fileActions, parent);
        return out;
    }
}
