// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#pragma once

#include <ibis/Models/Document.h>

#include <ftk/UI/Action.h>

namespace ibis
{
    namespace models
    {
        class Document;
    }

    class App;
    class MainWindow;

    //! Window actions.
    class WindowActions : public std::enable_shared_from_this<WindowActions>
    {
        FTK_NON_COPYABLE(WindowActions);

    protected:
        void _init(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<MainWindow>&);

        WindowActions() = default;

    public:
        ~WindowActions();

        static std::shared_ptr<WindowActions> create(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<MainWindow>&);

        const std::map<std::string, std::shared_ptr<ftk::Action> >& getActions() const;

    private:
        std::map<std::string, std::shared_ptr<ftk::Action> > _actions;

        std::shared_ptr<ftk::Observer<std::shared_ptr<models::Document> > > _currentObserver;
    };
}
