// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include "EditActions.h"

#include "App.h"
#include "MainWindow.h"

#include <ibis/Models/DocumentModel.h>

#include <ibis/Render/GraphCmd.h>

namespace ibis
{
    struct EditActions::Private
    {
        std::map<std::string, std::shared_ptr<ftk::Action> > actions;

        std::shared_ptr<ftk::Observer<std::shared_ptr<models::Document> > > currentObserver;
        std::shared_ptr<ftk::Observer<bool> > hasUndoObserver;
        std::shared_ptr<ftk::Observer<bool> > hasRedoObserver;
        std::shared_ptr<ftk::ListObserver<std::shared_ptr<render::INode> > > selectionObserver;
    };

    void EditActions::_init(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<MainWindow>& mainWindow)
    {
        FTK_P();

        auto appWeak = std::weak_ptr<App>(app);
        p.actions["Undo"] = ftk::Action::create(
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
        p.actions["Undo"]->setTooltip("Undo the last command.");

        p.actions["Redo"] = ftk::Action::create(
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
        p.actions["Redo"]->setTooltip("Redo the last command.");

        p.actions["SelectAll"] = ftk::Action::create(
            "Select All",
            ftk::KeyShortcut(ftk::Key::A, ftk::commandKeyModifier),
            [appWeak]
            {
                if (auto document = appWeak.lock()->getDocumentModel()->getCurrent())
                {
                    document->selectAll();
                }
            });
        p.actions["SelectAll"]->setTooltip("Select all nodes.");

        p.actions["SelectNone"] = ftk::Action::create(
            "Clear Selection",
            ftk::KeyShortcut(ftk::Key::A, ftk::commandKeyModifier, ftk::KeyModifier::Shift),
            [appWeak]
            {
                if (auto document = appWeak.lock()->getDocumentModel()->getCurrent())
                {
                    document->clearSelection();
                }
            });
        p.actions["SelectNone"]->setTooltip("Clear the selection.");

        p.actions["SelectInvert"] = ftk::Action::create(
            "Invert Selection",
            [appWeak]
            {
                if (auto document = appWeak.lock()->getDocumentModel()->getCurrent())
                {
                    document->invertSelection();
                }
            });
        p.actions["SelectInvert"]->setTooltip("Invert the selection.");

        p.actions["Delete"] = ftk::Action::create(
            "Delete",
            ftk::KeyShortcut(ftk::Key::Delete),
            [appWeak]
            {
                if (auto document = appWeak.lock()->getDocumentModel()->getCurrent())
                {
                    document->deleteSelection();
                }
            });
        p.actions["Delete"]->setTooltip("Delete the selection.");

        p.currentObserver = ftk::Observer<std::shared_ptr<models::Document> >::create(
            app->getDocumentModel()->observeCurrent(),
            [this](const std::shared_ptr<models::Document>& value)
            {
                FTK_P();
                p.hasUndoObserver.reset();
                p.hasRedoObserver.reset();
                p.selectionObserver.reset();
                if (value)
                {
                    p.actions["SelectAll"]->setEnabled(true);
                    p.actions["SelectInvert"]->setEnabled(true);

                    p.hasUndoObserver = ftk::Observer<bool>::create(
                        value->observeHasUndo(),
                        [this](bool value)
                        {
                            FTK_P();
                            p.actions["Undo"]->setEnabled(value);
                        });
                    p.hasRedoObserver = ftk::Observer<bool>::create(
                        value->observeHasRedo(),
                        [this](bool value)
                        {
                            FTK_P();
                            p.actions["Redo"]->setEnabled(value);
                        });
                    p.selectionObserver = ftk::ListObserver<std::shared_ptr<render::INode> >::create(
                        value->observeSelection(),
                        [this](const std::vector<std::shared_ptr<render::INode> >& value)
                        {
                            FTK_P();
                            p.actions["SelectNone"]->setEnabled(!value.empty());
                            p.actions["Delete"]->setEnabled(!value.empty());
                        });
                }
                else
                {
                    p.actions["Undo"]->setEnabled(false);
                    p.actions["Redo"]->setEnabled(false);
                    p.actions["SelectAll"]->setEnabled(false);
                    p.actions["SelectNone"]->setEnabled(false);
                    p.actions["SelectInvert"]->setEnabled(false);
                    p.actions["Delete"]->setEnabled(false);
                }
            });
    }

    EditActions::EditActions() :
        _p(new Private)
    {}

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
        return _p->actions;
    }
}