// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#pragma once

#include <ibis/Models/Document.h>

#include <ftk/UI/Menu.h>

namespace ibis
{
    class App;
    class TimeActions;

    //! Time menu.
    class TimeMenu : public ftk::Menu
    {
        FTK_NON_COPYABLE(TimeMenu);

    protected:
        void _init(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<TimeActions>&,
            const std::shared_ptr<IWidget>& parent);

        TimeMenu() = default;

    public:
        ~TimeMenu();

        static std::shared_ptr<TimeMenu> create(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<TimeActions>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

    private:
    };
}
