// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#pragma once

#include <ftk/UI/ToolBar.h>

namespace ibis
{
    class App;
    class EditActions;

    //! Edit tool bar.
    class EditToolBar : public ftk::ToolBar
    {
        FTK_NON_COPYABLE(EditToolBar);

    protected:
        void _init(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<EditActions>&,
            const std::shared_ptr<IWidget>& parent);

        EditToolBar();

    public:
        ~EditToolBar();

        static std::shared_ptr<EditToolBar> create(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<EditActions>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

    private:
        FTK_PRIVATE();
    };
}
