// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#pragma once

#include <ftk/UI/Menu.h>

namespace ibis
{
    class App;
    class FileActions;

    //! File menu.
    class FileMenu : public ftk::Menu
    {
        FTK_NON_COPYABLE(FileMenu);

    protected:
        void _init(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<FileActions>&,
            const std::shared_ptr<IWidget>& parent);

        FileMenu();

    public:
        ~FileMenu();

        static std::shared_ptr<FileMenu> create(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<FileActions>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

    private:
        FTK_PRIVATE();
    };
}
