// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#pragma once

#include <ibis/Models/Document.h>

#include <ftk/UI/MainWindow.h>
#include <ftk/UI/MenuBar.h>
#include <ftk/UI/RowLayout.h>
#include <ftk/UI/TabBar.h>
#include <ftk/UI/Splitter.h>
#include <ftk/UI/StackLayout.h>

namespace ibis
{
    class App;
    class DocumentWidget;
    class EditActions;
    class FileActions;
    class TimeActions;
    class ViewActions;
    class WindowActions;

    //! Main window.
    class MainWindow : public ftk::MainWindow
    {
        FTK_NON_COPYABLE(MainWindow);

    protected:
        void _init(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&);

        MainWindow() = default;

    public:
        ~MainWindow();

        static std::shared_ptr<MainWindow> create(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&);

        int getCurrentTab() const;

        void dropEvent(ftk::DragDropEvent&) override;

    private:
        std::shared_ptr<FileActions> _fileActions;
        std::shared_ptr<EditActions> _editActions;
        std::shared_ptr<TimeActions> _timeActions;
        std::shared_ptr<WindowActions> _windowActions;
        std::shared_ptr<ViewActions> _viewActions;
        std::map<std::shared_ptr<models::Document>, std::shared_ptr<DocumentWidget> > _widgets;
        std::shared_ptr<ftk::MenuBar> _menuBar;
        std::shared_ptr<ftk::TabBar> _tabBar;
        std::shared_ptr<ftk::StackLayout> _stackLayout;
        std::shared_ptr<ftk::VerticalLayout> _layout;

        std::shared_ptr<ftk::ListObserver<std::shared_ptr<models::Document> > > _documentsObserver;
        std::shared_ptr<ftk::Observer<int> > _currentObserver;
    };
}