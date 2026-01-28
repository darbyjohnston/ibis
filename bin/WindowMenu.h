// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#pragma once

#include <ibis/Models/Document.h>

#include <ftk/UI/Menu.h>

namespace ibis
{
    class App;
    class WindowActions;

    //! Window menu.
    class WindowMenu : public ftk::Menu
    {
        FTK_NON_COPYABLE(WindowMenu);

    protected:
        void _init(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<WindowActions>&,
            const std::shared_ptr<IWidget>& parent);

        WindowMenu() = default;

    public:
        ~WindowMenu();

        static std::shared_ptr<WindowMenu> create(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<WindowActions>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

    private:
    };
}
