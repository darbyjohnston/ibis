// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "SidePanel.h"

#include "App.h"

#include <ibis/UI/DiagWidget.h>
#include <ibis/UI/NodeBrowser.h>
#include <ibis/UI/NodeEditor.h>
#include <ibis/UI/SettingsWidget.h>
#include <ibis/UI/SysLogWidget.h>

#include <ftk/UI/Divider.h>
#include <ftk/UI/RowLayout.h>
#include <ftk/UI/TabBar.h>

namespace ibis
{
    struct SidePanel::Private
    {
        std::weak_ptr<App> app;
        std::shared_ptr<ftk::TabBar> tabBar;
        std::shared_ptr<ftk::IWidget> currentWidget;
        std::shared_ptr<ftk::VerticalLayout> layout;
    };

    void SidePanel::_init(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<ftk::IWidget>& parent)
    {
        IWidget::_init(context, "ibis::SidePanel", parent);
        FTK_P();
        
        p.app = app;

        p.tabBar = ftk::TabBar::create(context);
        p.tabBar->addTab("Node Browser");
        p.tabBar->addTab("Node Editor");
        p.tabBar->addTab("Settings");
        p.tabBar->addTab("Diagnostics");
        p.tabBar->addTab("System Log");

        p.layout = ftk::VerticalLayout::create(context, shared_from_this());
        p.layout->setSpacingRole(ftk::SizeRole::None);
        p.tabBar->setParent(p.layout);
        ftk::Divider::create(context, ftk::Orientation::Vertical, p.layout);

        _widgetUpdate(0);

        p.tabBar->setCurrentTabCallback(
            [this](int index)
            {
                _widgetUpdate(index);
            });
    }

    SidePanel::SidePanel() :
        _p(new Private)
    {}

    SidePanel::~SidePanel()
    {}

    std::shared_ptr<SidePanel> SidePanel::create(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<ftk::IWidget>& parent)
    {
        std::shared_ptr<SidePanel> out(new SidePanel);
        out->_init(context, app, parent);
        return out;
    }

    ftk::Size2I SidePanel::getSizeHint() const
    {
        return _p->layout->getSizeHint();
    }

    void SidePanel::setGeometry(const ftk::Box2I& value)
    {
        IWidget::setGeometry(value);
        _p->layout->setGeometry(value);
    }

    void SidePanel::_widgetUpdate(int index)
    {
        FTK_P();
        if (p.currentWidget)
        {
            p.currentWidget->setParent(nullptr);
            p.currentWidget.reset();
        }
        auto context = getContext();
        auto app = p.app.lock();
        switch (index)
        {
        case 0:
            p.currentWidget = ui::NodeBrowser::create(
                context,
                app->getNodeFactory(),
                p.layout);
            break;
        case 1:
            p.currentWidget = ui::NodeEditor::create(
                context,
                app->getNodeWidgetFactory(),
                app->getDocumentModel(),
                p.layout);
            break;
        case 2:
            p.currentWidget = ui::SettingsWidget::create(context, p.layout);
            break;
        case 3:
            p.currentWidget = ui::DiagWidget::create(context, p.layout);
            break;
        case 4:
            p.currentWidget = ui::SysLogWidget::create(context, p.layout);
            break;
        default: break;
        }
        if (p.currentWidget)
        {
            p.currentWidget->setVStretch(ftk::Stretch::Expanding);
        }
    }
}
