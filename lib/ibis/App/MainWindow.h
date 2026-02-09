// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#pragma once

#include <ibis/App/SidePanel.h>

#include <ftk/UI/MainWindow.h>

namespace ibis
{
    class App;
    class DocumentWidget;

    //! Main window.
    class MainWindow : public ftk::MainWindow
    {
        FTK_NON_COPYABLE(MainWindow);

    protected:
        void _init(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&,
            const ftk::Size2I&);

        MainWindow();

    public:
        ~MainWindow();

        static std::shared_ptr<MainWindow> create(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&,
            const ftk::Size2I&);

        int getCurrentTab() const;
        std::shared_ptr<DocumentWidget> getDocumentWidget() const;
        std::shared_ptr<ftk::IObservable<std::shared_ptr<DocumentWidget> > > observeDocumentWidget() const;

        SidePanel getSidePanel() const;
        std::shared_ptr<ftk::IObservable<std::pair<SidePanel, bool> > > observeSidePanel() const;
        void setSidePanel(SidePanel);

        void dragEnterEvent(ftk::DragDropEvent&) override;
        void dragLeaveEvent(ftk::DragDropEvent&) override;
        void dragMoveEvent(ftk::DragDropEvent&) override;
        void dropEvent(ftk::DragDropEvent&) override;

    private:
        FTK_PRIVATE();
    };
}