// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#pragma once

#include <ftk/UI/ToolBar.h>

namespace ibis
{
    class App;
    class WindowActions;

    //! Window tool bar.
    class WindowToolBar : public ftk::ToolBar
    {
        FTK_NON_COPYABLE(WindowToolBar);

    protected:
        void _init(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<WindowActions>&,
            const std::shared_ptr<IWidget>& parent);

        WindowToolBar();

    public:
        ~WindowToolBar();

        static std::shared_ptr<WindowToolBar> create(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<WindowActions>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

    private:
        FTK_PRIVATE();
    };
}
