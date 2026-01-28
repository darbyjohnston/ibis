// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#pragma once

#include <ftk/UI/Action.h>

namespace ibis
{
    namespace render
    {
        class INode;
    }

    class App;
    class MainWindow;

    //! Edit actions.
    class EditActions : public std::enable_shared_from_this<EditActions>
    {
        FTK_NON_COPYABLE(EditActions);

    protected:
        void _init(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<MainWindow>&);

        EditActions();

    public:
        ~EditActions();

        static std::shared_ptr<EditActions> create(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<MainWindow>&);

        const std::map<std::string, std::shared_ptr<ftk::Action> >& getActions() const;

    private:
        FTK_PRIVATE();
    };
}
