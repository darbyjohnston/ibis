// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include "EditActions.h"

#include "App.h"
#include "MainWindow.h"

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

        _actions["SelectAll"] = ftk::Action::create(
            "Select All",
            ftk::KeyShortcut(ftk::Key::A, ftk::commandKeyModifier),
            [appWeak]
            {
                if (auto document = appWeak.lock()->getDocumentModel()->getCurrent())
                {
                    document->selectAll();
                }
            });
        _actions["SelectAll"]->setTooltip("Select all nodes.");

        _actions["SelectNone"] = ftk::Action::create(
            "Clear Selection",
            ftk::KeyShortcut(ftk::Key::A, ftk::commandKeyModifier, ftk::KeyModifier::Shift),
            [appWeak]
            {
                if (auto document = appWeak.lock()->getDocumentModel()->getCurrent())
                {
                    document->clearSelection();
                }
            });
        _actions["SelectNone"]->setTooltip("Clear the selection.");

        _actions["SelectInvert"] = ftk::Action::create(
            "Invert Selection",
            [appWeak]
            {
                if (auto document = appWeak.lock()->getDocumentModel()->getCurrent())
                {
                    document->invertSelection();
                }
            });
        _actions["SelectInvert"]->setTooltip("Invert the selection.");

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
                    _actions["SelectAll"]->setEnabled(true);
                    _actions["SelectInvert"]->setEnabled(true);

                    _hasUndoObserver = ftk::Observer<bool>::create(
                        value->observeHasUndo(),
                        [this](bool value)
                        {
                            _actions["Undo"]->setEnabled(value);
                        });
                    _hasRedoObserver = ftk::Observer<bool>::create(
                        value->observeHasRedo(),
                        [this](bool value)
                        {
                            _actions["Redo"]->setEnabled(value);
                        });
                    _selectionObserver = ftk::ListObserver<std::shared_ptr<render::INode> >::create(
                        value->observeSelection(),
                        [this](const std::vector<std::shared_ptr<render::INode> >& value)
                        {
                            _actions["SelectNone"]->setEnabled(!value.empty());
                            _actions["Delete"]->setEnabled(!value.empty());
                        });
                }
                else
                {
                    _actions["Undo"]->setEnabled(false);
                    _actions["Redo"]->setEnabled(false);
                    _actions["SelectAll"]->setEnabled(false);
                    _actions["SelectNone"]->setEnabled(false);
                    _actions["SelectInvert"]->setEnabled(false);
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