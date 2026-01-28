// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#pragma once

#include <ftk/UI/ToolBar.h>

namespace ibis
{
    class App;
    class ViewActions;

    //! View tool bar.
    class ViewToolBar : public ftk::ToolBar
    {
        FTK_NON_COPYABLE(ViewToolBar);

    protected:
        void _init(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<ViewActions>&,
            const std::shared_ptr<IWidget>& parent);

        ViewToolBar();

    public:
        ~ViewToolBar();

        static std::shared_ptr<ViewToolBar> create(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<ViewActions>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

    private:
        FTK_PRIVATE();
    };
}
