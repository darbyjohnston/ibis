// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <ibis/Models/Document.h>

#include <ftk/UI/Menu.h>

namespace ibis
{
    class App;
    class EditActions;

    //! Edit menu.
    class EditMenu : public ftk::Menu
    {
        FTK_NON_COPYABLE(EditMenu);

    protected:
        void _init(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<EditActions>&,
            const std::shared_ptr<IWidget>& parent);

        EditMenu() = default;

    public:
        ~EditMenu();

        static std::shared_ptr<EditMenu> create(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<EditActions>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

    private:
    };
}
