// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#pragma once

#include <ftk/UI/ToolBar.h>

namespace ibis
{
    class App;
    class FileActions;

    //! File tool bar.
    class FileToolBar : public ftk::ToolBar
    {
        FTK_NON_COPYABLE(FileToolBar);

    protected:
        void _init(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<FileActions>&,
            const std::shared_ptr<IWidget>& parent);

        FileToolBar();

    public:
        ~FileToolBar();

        static std::shared_ptr<FileToolBar> create(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<FileActions>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

    private:
        FTK_PRIVATE();
    };
}
