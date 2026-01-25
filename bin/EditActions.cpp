// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include "EditActions.h"

#include "App.h"
#include "MainWindow.h"

#include <ibis/Models/NodeSelectionModel.h>

#include <ibis/Render/GraphCmd.h>

namespace ibis
{
    void EditActions::_init(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<MainWindow>& mainWindow)
    {
        auto appWeak = std::weak_ptr<App>(app);
        _actions["Undo"] = ftk::Action::create(
            "Undo",
            "Undo",
            ftk::KeyShortcut(ftk::Key::Z, ftk::commandKeyModifier),
            [appWeak]
            {
                if (auto document = appWeak.lock()->getDocumentModel()->getCurrent())
                {
                    document->undo();
                }
            });
        _actions["Undo"]->setTooltip("Undo the last command.");

        _actions["Redo"] = ftk::Action::create(
            "Redo",
            "Redo",
            ftk::KeyShortcut(ftk::Key::Z, ftk::commandKeyModifier, ftk::KeyModifier::Shift),
            [appWeak]
            {
                if (auto document = appWeak.lock()->getDocumentModel()->getCurrent())
                {
                    document->redo();
                }
            });
        _actions["Redo"]->setTooltip("Redo the last command.");

        _actions["Delete"] = ftk::Action::create(
            "Delete",
            ftk::KeyShortcut(ftk::Key::Delete),
            [appWeak]
            {
                if (auto document = appWeak.lock()->getDocumentModel()->getCurrent())
                {
                    document->deleteSelection();
                }
            });
        _actions["Delete"]->setTooltip("Delete the selection.");

        _currentObserver = ftk::Observer<std::shared_ptr<models::Document> >::create(
            app->getDocumentModel()->observeCurrent(),
            [this](const std::shared_ptr<models::Document>& value)
            {
                _hasUndoObserver.reset();
                _hasRedoObserver.reset();
                _selectionObserver.reset();
                if (value)
                {
                    _hasUndoObserver = ftk::Observer<bool>::create(
                        value->getCommandStack()->observeHasUndo(),
                        [this](bool value)
                        {
                            _actions["Undo"]->setEnabled(value);
                        });
                    _hasRedoObserver = ftk::Observer<bool>::create(
                        value->getCommandStack()->observeHasRedo(),
                        [this](bool value)
                        {
                            _actions["Redo"]->setEnabled(value);
                        });
                    _selectionObserver = ftk::ListObserver<std::shared_ptr<render::INode> >::create(
                        value->getSelectionModel()->observe(),
                        [this](const std::vector<std::shared_ptr<render::INode> >& value)
                        {
                            _actions["Delete"]->setEnabled(!value.empty());
                        });
                }
                else
                {
                    _actions["Undo"]->setEnabled(false);
                    _actions["Redo"]->setEnabled(false);
                    _actions["Delete"]->setEnabled(false);
                }
            });
    }

    EditActions::~EditActions()
    {}

    std::shared_ptr<EditActions> EditActions::create(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<MainWindow>& mainWindow)
    {
        auto out = std::shared_ptr<EditActions>(new EditActions);
        out->_init(context, app, mainWindow);
        return out;
    }

    const std::map<std::string, std::shared_ptr<ftk::Action> >& EditActions::getActions() const
    {
        return _actions;
    }
}