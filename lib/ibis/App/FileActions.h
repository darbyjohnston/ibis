// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#pragma once

#include <ftk/UI/Action.h>

namespace ibis
{
    class App;
    class MainWindow;

    //! File actions.
    class FileActions : public std::enable_shared_from_this<FileActions>
    {
        FTK_NON_COPYABLE(FileActions);

    protected:
        void _init(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<MainWindow>&);

        FileActions();

    public:
        ~FileActions();

        static std::shared_ptr<FileActions> create(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<MainWindow>&);

        const std::map<std::string, std::shared_ptr<ftk::Action> >& getActions() const;

    private:
        FTK_PRIVATE();
    };
}
