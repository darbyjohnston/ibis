// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#pragma once

#include <ftk/UI/MainWindow.h>

namespace ibis
{
    class App;

    //! Main window.
    class MainWindow : public ftk::MainWindow
    {
        FTK_NON_COPYABLE(MainWindow);

    protected:
        void _init(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&);

        MainWindow();

    public:
        ~MainWindow();

        static std::shared_ptr<MainWindow> create(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&);

        int getCurrentTab() const;

        void dropEvent(ftk::DragDropEvent&) override;

    private:
        FTK_PRIVATE();
    };
}