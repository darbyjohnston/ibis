// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include "MainWindow.h"

#include "App.h"
#include "DocumentWidget.h"
#include "EditActions.h"
#include "EditMenu.h"
#include "EditToolBar.h"
#include "FileActions.h"
#include "FileMenu.h"
#include "FileToolBar.h"
#include "TimeActions.h"
#include "TimeMenu.h"
#include "ViewActions.h"
#include "ViewMenu.h"
#include "ViewToolBar.h"
#include "WindowActions.h"
#include "WindowMenu.h"
#include "WindowToolBar.h"

#include <ibis/Models/DocumentModel.h>

#include <ftk/UI/Divider.h>
#include <ftk/UI/IconSystem.h>
#include <ftk/UI/Menu.h>
#include <ftk/UI/MenuBar.h>
#include <ftk/UI/RowLayout.h>
#include <ftk/UI/TabBar.h>
#include <ftk/UI/ToolBar.h>
#include <ftk/UI/Splitter.h>
#include <ftk/UI/StackLayout.h>

namespace ibis
{
    struct MainWindow::Private
    {
        std::shared_ptr<ftk::Observable<std::shared_ptr<DocumentWidget> > > currentWidget;

        std::shared_ptr<FileActions> fileActions;
        std::shared_ptr<EditActions> editActions;
        std::shared_ptr<TimeActions> timeActions;
        std::shared_ptr<WindowActions> windowActions;
        std::shared_ptr<ViewActions> viewActions;
        std::map<std::shared_ptr<models::Document>, std::shared_ptr<DocumentWidget> > widgets;
        std::shared_ptr<ftk::MenuBar> menuBar;
        std::shared_ptr<ftk::TabBar> tabBar;
        std::shared_ptr<ftk::StackLayout> stackLayout;
        std::shared_ptr<ftk::VerticalLayout> layout;

        std::shared_ptr<ftk::ListObserver<std::shared_ptr<models::Document> > > documentsObserver;
        std::shared_ptr<ftk::Observer<std::shared_ptr<models::Document> > > currentObserver;
        std::shared_ptr<ftk::Observer<int> > currentIndexObserver;
    };

    void MainWindow::_init(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app)
    {
        ftk::MainWindow::_init(context, app, ftk::Size2I(1700, 960));
        FTK_P();

        auto iconSystem = context->getSystem<ftk::IconSystem>();
        setIcon(iconSystem->get("ibis", 1.0));

        p.currentWidget = ftk::Observable<std::shared_ptr<DocumentWidget> >::create();

        auto mainWindow = std::dynamic_pointer_cast<MainWindow>(shared_from_this());
        p.fileActions = FileActions::create(context, app, mainWindow);
        p.editActions = EditActions::create(context, app, mainWindow);
        p.timeActions = TimeActions::create(context, app);
        p.windowActions = WindowActions::create(context, app, mainWindow);
        p.viewActions = ViewActions::create(context, app, mainWindow);

        p.menuBar = ftk::MenuBar::create(context);
        p.menuBar->addMenu("File", FileMenu::create(context, app, p.fileActions));
        p.menuBar->addMenu("Edit", EditMenu::create(context, app, p.editActions));
        p.menuBar->addMenu("Time", TimeMenu::create(context, app, p.timeActions));
        p.menuBar->addMenu("Window", WindowMenu::create(context, app, p.windowActions));
        p.menuBar->addMenu("View", ViewMenu::create(context, app, p.viewActions));
        setMenuBar(p.menuBar);

        auto fileToolBar = FileToolBar::create(context, app, p.fileActions);
        auto editToolBar = EditToolBar::create(context, app, p.editActions);
        auto windowToolBar = WindowToolBar::create(context, app, p.windowActions);
        auto viewToolBar = ViewToolBar::create(context, app, p.viewActions);

        p.tabBar = ftk::TabBar::create(context);
        p.tabBar->setTabsClosable(true);

        p.stackLayout = ftk::StackLayout::create(context);
        p.stackLayout->setVStretch(ftk::Stretch::Expanding);

        p.layout = ftk::VerticalLayout::create(context);
        p.layout->setSpacingRole(ftk::SizeRole::None);
        auto hLayout = ftk::HorizontalLayout::create(context, p.layout);
        fileToolBar->setParent(hLayout);
        ftk::Divider::create(context, ftk::Orientation::Horizontal, hLayout);
        editToolBar->setParent(hLayout);
        ftk::Divider::create(context, ftk::Orientation::Horizontal, hLayout);
        windowToolBar->setParent(hLayout);
        ftk::Divider::create(context, ftk::Orientation::Horizontal, hLayout);
        viewToolBar->setParent(hLayout);
        ftk::Divider::create(context, ftk::Orientation::Vertical, p.layout);
        p.tabBar->setParent(p.layout);
        ftk::Divider::create(context, ftk::Orientation::Vertical, p.layout);
        p.stackLayout->setParent(p.layout);
        setWidget(p.layout);

        std::weak_ptr<App> appWeak(app);
        p.tabBar->setCurrentTabCallback(
            [appWeak](int index)
            {
                appWeak.lock()->getDocumentModel()->setCurrent(index);
            });

        p.tabBar->setTabCloseCallback(
            [appWeak](int index)
            {
                appWeak.lock()->getDocumentModel()->close(index);
            });

        p.documentsObserver = ftk::ListObserver<std::shared_ptr<models::Document> >::create(
            app->getDocumentModel()->observe(),
            [this, appWeak](const std::vector<std::shared_ptr<models::Document> >& value)
            {
                FTK_P();
                auto currentWidget = p.stackLayout->getCurrentWidget();
                p.tabBar->clear();
                p.stackLayout->clear();

                std::map<std::shared_ptr<models::Document>, std::shared_ptr<DocumentWidget> > widgets;
                for (const auto& document : value)
                {
                    const auto& path = document->getPath();
                    const std::string fileName = path.filename().u8string();
                    p.tabBar->addTab(fileName, path.u8string());

                    std::shared_ptr<DocumentWidget> widget;
                    auto i = p.widgets.find(document);
                    if (i != p.widgets.end())
                    {
                        widget = i->second;
                    }
                    else
                    {
                        widget = DocumentWidget::create(
                            getContext(),
                            appWeak.lock(),
                            document);
                    }
                    widgets[document] = widget;
                    widget->setParent(p.stackLayout);
                }

                p.widgets = widgets;
                p.stackLayout->setCurrentWidget(currentWidget);
            });

        p.currentObserver = ftk::Observer<std::shared_ptr<models::Document> >::create(
            app->getDocumentModel()->observeCurrent(),
            [this](const std::shared_ptr<models::Document>& value)
            {
                FTK_P();
                std::shared_ptr<DocumentWidget> currentWidget;
                const auto i = p.widgets.find(value);
                p.currentWidget->setIfChanged(i != p.widgets.end() ? i->second : nullptr);
            });

        p.currentIndexObserver = ftk::Observer<int>::create(
            app->getDocumentModel()->observeCurrentIndex(),
            [this](int value)
            {
                FTK_P();
                p.tabBar->setCurrentTab(value);
                p.stackLayout->setCurrentIndex(value);
            });
    }

    MainWindow::MainWindow() :
        _p(new Private)
    {}

    MainWindow::~MainWindow()
    {}

    std::shared_ptr<MainWindow> MainWindow::create(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app)
    {
        auto out = std::shared_ptr<MainWindow>(new MainWindow);
        out->_init(context, app);
        return out;
    }

    int MainWindow::getCurrentTab() const
    {
        return _p->tabBar->getCurrentTab();
    }

    std::shared_ptr<DocumentWidget> MainWindow::getDocumentWidget() const
    {
        return _p->currentWidget->get();
    }

    std::shared_ptr<ftk::IObservable<std::shared_ptr<DocumentWidget> > > MainWindow::observeDocumentWidget() const
    {
        return _p->currentWidget;
    }

    void MainWindow::dropEvent(ftk::DragDropEvent& event)
    {
        event.accept = true;
        if (auto textData = std::dynamic_pointer_cast<ftk::DragDropTextData>(event.data))
        {
            if (auto app = getApp())
            {
                for (const auto& fileName : textData->getText())
                {
                    //app->open(ftk::Path(fileName));
                }
            }
        }
    }
}