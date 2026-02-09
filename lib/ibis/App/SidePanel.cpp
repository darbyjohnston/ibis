// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "SidePanel.h"

#include "App.h"

#include <ibis/UI/DiagWidget.h>
#include <ibis/UI/DocumentEditor.h>
#include <ibis/UI/MessagesWidget.h>
#include <ibis/UI/NodeBrowser.h>
#include <ibis/UI/NodeEditor.h>
#include <ibis/UI/SettingsWidget.h>
#include <ibis/UI/SysLogWidget.h>

#include <ftk/UI/Divider.h>
#include <ftk/UI/RowLayout.h>
#include <ftk/UI/TabBar.h>
#include <ftk/UI/ToolButton.h>
#include <ftk/Core/Error.h>
#include <ftk/Core/Format.h>
#include <ftk/Core/String.h>

namespace ibis
{
    FTK_ENUM_IMPL(
        SidePanel,
        "Node Browser",
        "Node Editor",
        "Document",
        "Settings",
        "Messages",
        "Diagnostics",
        "System Log");

    struct SidePanelWidget::Private
    {
        std::weak_ptr<App> app;
        std::shared_ptr<ftk::TabBar> tabBar;
        std::shared_ptr<ftk::IWidget> currentWidget;
        std::shared_ptr<ftk::VerticalLayout> layout;
        std::function<void(SidePanel)> callback;
        std::function<void(void)> closeCallback;
    };

    void SidePanelWidget::_init(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<ftk::IWidget>& parent)
    {
        IWidget::_init(context, "ibis::SidePanelWidget", parent);
        FTK_P();
        
        p.app = app;

        p.tabBar = ftk::TabBar::create(context);
        p.tabBar->setHStretch(ftk::Stretch::Expanding);
        for (const auto& i : getSidePanelLabels())
        {
            p.tabBar->addTab(i);
        }

        auto closeButton = ftk::ToolButton::create(context);
        closeButton->setIcon("Close");
        closeButton->setTooltip("Close the side panel.");

        p.layout = ftk::VerticalLayout::create(context, shared_from_this());
        p.layout->setSpacingRole(ftk::SizeRole::None);
        auto hLayout = ftk::HorizontalLayout::create(context, p.layout);
        hLayout->setSpacingRole(ftk::SizeRole::None);
        p.tabBar->setParent(hLayout);
        closeButton->setParent(hLayout);
        ftk::Divider::create(context, ftk::Orientation::Vertical, p.layout);

        _widgetUpdate(SidePanel::First);

        p.tabBar->setCallback(
            [this](int index)
            {
                FTK_P();
                _widgetUpdate(static_cast<SidePanel>(index));
                if (p.callback)
                {
                    p.callback(static_cast<SidePanel>(index));
                }
            });

        closeButton->setClickedCallback(
            [this]
            {
                FTK_P();
                if (p.closeCallback)
                {
                    p.closeCallback();
                }
            });
    }

    SidePanelWidget::SidePanelWidget() :
        _p(new Private)
    {}

    SidePanelWidget::~SidePanelWidget()
    {}

    std::shared_ptr<SidePanelWidget> SidePanelWidget::create(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<ftk::IWidget>& parent)
    {
        std::shared_ptr<SidePanelWidget> out(new SidePanelWidget);
        out->_init(context, app, parent);
        return out;
    }

    void SidePanelWidget::setSidePanel(SidePanel value)
    {
        _p->tabBar->setCurrent(static_cast<int>(value));
        _widgetUpdate(value);
    }

    void SidePanelWidget::setCallback(const std::function<void(SidePanel)>& value)
    {
        _p->callback = value;
    }

    void SidePanelWidget::setCloseCallback(const std::function<void(void)>& value)
    {
        _p->closeCallback = value;
    }

    ftk::Size2I SidePanelWidget::getSizeHint() const
    {
        return _p->layout->getSizeHint();
    }

    void SidePanelWidget::setGeometry(const ftk::Box2I& value)
    {
        IWidget::setGeometry(value);
        _p->layout->setGeometry(value);
    }

    void SidePanelWidget::_widgetUpdate(SidePanel value)
    {
        FTK_P();
        if (p.currentWidget)
        {
            p.currentWidget->setParent(nullptr);
            p.currentWidget.reset();
        }
        auto context = getContext();
        auto app = p.app.lock();
        switch (value)
        {
        case SidePanel::NodeBrowser:
            p.currentWidget = ui::NodeBrowser::create(
                context,
                app->getNodeFactory(),
                p.layout);
            break;
        case SidePanel::NodeEditor:
            p.currentWidget = ui::NodeEditor::create(
                context,
                app->getNodeWidgetFactory(),
                app->getDocumentModel(),
                p.layout);
            break;
        case SidePanel::Document:
            p.currentWidget = ui::DocumentEditor::create(
                context,
                app->getDocumentModel(),
                p.layout);
            break;
        case SidePanel::Settings:
            p.currentWidget = ui::SettingsWidget::create(
                context,
                app->getSettingsModel(),
                p.layout);
            break;
        case SidePanel::Messages:
            p.currentWidget = ui::MessagesWidget::create(
                context,
                app->getSettingsModel(),
                app->getMessagesModel(),
                p.layout);
            break;
        case SidePanel::Diag:
            p.currentWidget = ui::DiagWidget::create(context, p.layout);
            break;
        case SidePanel::SysLog:
            p.currentWidget = ui::SysLogWidget::create(
                context,
                app->getSettingsModel(),
                app->getMessagesModel(),
                p.layout);
            break;
        default: break;
        }
        if (p.currentWidget)
        {
            p.currentWidget->setVStretch(ftk::Stretch::Expanding);
        }
    }
}
