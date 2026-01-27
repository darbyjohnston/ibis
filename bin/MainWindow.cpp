// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include "MainWindow.h"

#include "App.h"
#include "DocumentWidget.h"
#include "EditActions.h"
#include "EditMenu.h"
#include "FileActions.h"
#include "FileMenu.h"
#include "TimeActions.h"
#include "TimeMenu.h"

#include <ftk/UI/Divider.h>
#include <ftk/UI/IconSystem.h>
#include <ftk/UI/Menu.h>
#include <ftk/UI/ToolBar.h>

namespace ibis
{
    void MainWindow::_init(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app)
    {
        ftk::MainWindow::_init(context, app, ftk::Size2I(1700, 960));

        auto iconSystem = context->getSystem<ftk::IconSystem>();
        setIcon(iconSystem->get("ibis", 1.0));

        auto mainWindow = std::dynamic_pointer_cast<MainWindow>(shared_from_this());
        _fileActions = FileActions::create(context, app, mainWindow);
        _editActions = EditActions::create(context, app, mainWindow);
        _timeActions = TimeActions::create(context, app);

        _menuBar = ftk::MenuBar::create(context);
        _menuBar->addMenu("File", FileMenu::create(context, app, _fileActions));
        _menuBar->addMenu("Edit", EditMenu::create(context, app, _editActions));
        _menuBar->addMenu("Time", TimeMenu::create(context, app, _timeActions));
        setMenuBar(_menuBar);

        _tabBar = ftk::TabBar::create(context);
        _tabBar->setTabsClosable(true);

        _stackLayout = ftk::StackLayout::create(context);
        _stackLayout->setVStretch(ftk::Stretch::Expanding);

        _layout = ftk::VerticalLayout::create(context);
        _layout->setSpacingRole(ftk::SizeRole::None);
        _tabBar->setParent(_layout);
        ftk::Divider::create(context, ftk::Orientation::Vertical, _layout);
        _stackLayout->setParent(_layout);
        setWidget(_layout);

        std::weak_ptr<App> appWeak(app);
        _tabBar->setCurrentTabCallback(
            [appWeak](int index)
            {
                appWeak.lock()->getDocumentModel()->setCurrent(index);
            });

        _tabBar->setTabCloseCallback(
            [appWeak](int index)
            {
                appWeak.lock()->getDocumentModel()->close(index);
            });

        _documentsObserver = ftk::ListObserver<std::shared_ptr<models::Document> >::create(
            app->getDocumentModel()->observe(),
            [this, appWeak](const std::vector<std::shared_ptr<models::Document> >& value)
            {
                auto currentWidget = _stackLayout->getCurrentWidget();
                _tabBar->clear();
                _stackLayout->clear();

                std::map<std::shared_ptr<models::Document>, std::shared_ptr<DocumentWidget> > widgets;
                for (const auto& document : value)
                {
                    const auto& path = document->getPath();
                    const std::string fileName = path.filename().u8string();
                    _tabBar->addTab(fileName, path.u8string());

                    std::shared_ptr<DocumentWidget> widget;
                    auto i = _widgets.find(document);
                    if (i != _widgets.end())
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
                    widget->setParent(_stackLayout);
                }

                _widgets = widgets;
                _stackLayout->setCurrentWidget(currentWidget);
            });

        _currentObserver = ftk::Observer<int>::create(
            app->getDocumentModel()->observeCurrentIndex(),
            [this](int value)
            {
                _tabBar->setCurrentTab(value);
                _stackLayout->setCurrentIndex(value);
            });
    }

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
        return _tabBar->getCurrentTab();
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