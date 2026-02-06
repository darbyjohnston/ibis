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
#include "NodeMenu.h"
#include "SidePanel.h"
#include "TimeActions.h"
#include "TimeMenu.h"
#include "ViewActions.h"
#include "ViewMenu.h"
#include "ViewToolBar.h"
#include "WindowActions.h"
#include "WindowMenu.h"
#include "WindowToolBar.h"

#include <ibis/Models/DocumentModel.h>

#include <ibis/Render/GraphCmd.h>
#include <ibis/Render/InputNode.h>
#include <ibis/Render/NodeFactory.h>

#include <ftk/UI/Divider.h>
#include <ftk/UI/IconSystem.h>
#include <ftk/UI/Menu.h>
#include <ftk/UI/MenuBar.h>
#include <ftk/UI/RowLayout.h>
#include <ftk/UI/Splitter.h>
#include <ftk/UI/TabBar.h>
#include <ftk/UI/ToolBar.h>
#include <ftk/UI/Splitter.h>
#include <ftk/UI/StackLayout.h>

namespace ibis
{
    struct MainWindow::Private
    {
        std::shared_ptr<ftk::Observable<std::shared_ptr<DocumentWidget> > > documentWidget;
        std::shared_ptr<ftk::Observable<std::pair<SidePanel, bool> > > sidePanel;

        std::shared_ptr<FileActions> fileActions;
        std::shared_ptr<EditActions> editActions;
        std::shared_ptr<TimeActions> timeActions;
        std::shared_ptr<WindowActions> windowActions;
        std::shared_ptr<ViewActions> viewActions;
        std::map<std::shared_ptr<models::Document>, std::shared_ptr<DocumentWidget> > widgets;
        std::shared_ptr<ftk::MenuBar> menuBar;
        std::shared_ptr<ftk::TabBar> documentTabBar;
        std::shared_ptr<ftk::StackLayout> documentLayout;
        std::shared_ptr<SidePanelWidget> sidePanelWidget;
        std::shared_ptr<ftk::Splitter> splitter;
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

        p.documentWidget = ftk::Observable<std::shared_ptr<DocumentWidget> >::create();

        p.sidePanel = ftk::Observable<std::pair<SidePanel, bool> >::create(
            std::make_pair(SidePanel::NodeBrowser, true));

        p.sidePanelWidget = SidePanelWidget::create(context, app);

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
        p.menuBar->addMenu("Node", NodeMenu::create(context, app));
        setMenuBar(p.menuBar);

        auto fileToolBar = FileToolBar::create(context, app, p.fileActions);
        auto editToolBar = EditToolBar::create(context, app, p.editActions);
        auto windowToolBar = WindowToolBar::create(context, app, p.windowActions);
        auto viewToolBar = ViewToolBar::create(context, app, p.viewActions);

        p.documentTabBar = ftk::TabBar::create(context);
        p.documentTabBar->setTabsClosable(true);

        p.documentLayout = ftk::StackLayout::create(context);
        p.documentLayout->setVStretch(ftk::Stretch::Expanding);

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
        p.splitter = ftk::Splitter::create(context, ftk::Orientation::Horizontal, p.layout);
        p.splitter->setSplit(.8F);
        auto vLayout = ftk::VerticalLayout::create(context, p.splitter);
        vLayout->setSpacingRole(ftk::SizeRole::None);
        p.documentTabBar->setParent(vLayout);
        ftk::Divider::create(context, ftk::Orientation::Vertical, p.layout);
        p.documentLayout->setParent(vLayout);
        p.sidePanelWidget->setParent(p.splitter);

        setWidget(p.layout);

        std::weak_ptr<App> appWeak(app);
        p.documentTabBar->setCurrentTabCallback(
            [appWeak](int index)
            {
                appWeak.lock()->getDocumentModel()->setCurrent(index);
            });

        p.documentTabBar->setTabCloseCallback(
            [appWeak](int index)
            {
                appWeak.lock()->getDocumentModel()->close(index);
            });

        p.documentsObserver = ftk::ListObserver<std::shared_ptr<models::Document> >::create(
            app->getDocumentModel()->observe(),
            [this, appWeak](const std::vector<std::shared_ptr<models::Document> >& value)
            {
                FTK_P();
                auto currentWidget = p.documentLayout->getCurrentWidget();
                p.documentTabBar->clear();
                p.documentLayout->clear();

                std::map<std::shared_ptr<models::Document>, std::shared_ptr<DocumentWidget> > widgets;
                for (const auto& document : value)
                {
                    const auto& path = document->getPath();
                    const std::string fileName = path.filename().u8string();
                    p.documentTabBar->addTab(fileName, path.u8string());

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
                            document,
                            p.editActions->getActions());
                    }
                    widgets[document] = widget;
                    widget->setParent(p.documentLayout);
                }

                p.widgets = widgets;
                p.documentLayout->setCurrentWidget(currentWidget);
            });

        p.currentObserver = ftk::Observer<std::shared_ptr<models::Document> >::create(
            app->getDocumentModel()->observeCurrent(),
            [this](const std::shared_ptr<models::Document>& value)
            {
                FTK_P();
                const auto i = p.widgets.find(value);
                p.documentWidget->setIfChanged(i != p.widgets.end() ? i->second : nullptr);
            });

        p.currentIndexObserver = ftk::Observer<int>::create(
            app->getDocumentModel()->observeCurrentIndex(),
            [this](int value)
            {
                FTK_P();
                p.documentTabBar->setCurrentTab(value);
                p.documentLayout->setCurrentIndex(value);
            });

        p.sidePanelWidget->setCallback(
            [this](SidePanel value)
            {
                FTK_P();
                auto pair = p.sidePanel->get();
                pair.first = value;
                p.sidePanel->setIfChanged(pair);
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
        return _p->documentTabBar->getCurrentTab();
    }

    std::shared_ptr<DocumentWidget> MainWindow::getDocumentWidget() const
    {
        return _p->documentWidget->get();
    }

    std::shared_ptr<ftk::IObservable<std::shared_ptr<DocumentWidget> > > MainWindow::observeDocumentWidget() const
    {
        return _p->documentWidget;
    }

    std::shared_ptr<ftk::IObservable<std::pair<SidePanel, bool> > > MainWindow::observeSidePanel() const
    {
        return _p->sidePanel;
    }

    void MainWindow::setSidePanel(SidePanel value)
    {
        FTK_P();
        const auto prev = p.sidePanel->get();
        bool visible = p.sidePanelWidget->isVisible(false);
        if (value == prev.first)
        {
            visible = !visible;
        }
        else
        {
            visible = true;
        }
        p.sidePanel->setIfChanged(std::make_pair(value, visible));
        p.sidePanelWidget->setVisible(visible);
        p.sidePanelWidget->setSidePanel(value);
    }

    void MainWindow::dropEvent(ftk::DragDropEvent& event)
    {
        event.accept = true;
        if (auto textData = std::dynamic_pointer_cast<ftk::DragDropTextData>(event.data))
        {
            auto context = getContext();
            auto app = std::dynamic_pointer_cast<App>(getApp());

            auto document = app->getDocumentModel()->getCurrent();
            if (!document)
            {
                app->newDocument();
                document = app->getDocumentModel()->getCurrent();
            }

            ftk::V2I pos = event.pos;
            if (auto documentWidget = app->getMainWindow()->getDocumentWidget())
            {
                pos = pos -
                    documentWidget->getCanvasViewportRect().min +
                    documentWidget->getCanvasViewRect().min;
            }

            if (document)
            {
                for (const auto& fileName : textData->getText())
                {
                    ftk::Path path(fileName);
                    const std::string ext = path.getExt();
                    if (".ibis" == ext)
                    {
                        app->open(path);
                    }
                    else
                    {
                        if (auto node = render::createInputNode(context, fileName))
                        {
                            document->command(
                                render::AddNodesCmd::create(
                                    document->getGraph(),
                                    { node },
                                    { pos }));
                            pos.x += 100;
                            pos.y += 100;
                        }
                    }
                }
            }
        }
    }
}