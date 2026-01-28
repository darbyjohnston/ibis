// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include "DocumentWidget.h"

#include "App.h"

#include <ibis/UI/NodeBrowser.h>
#include <ibis/UI/NodeEditor.h>
#include <ibis/UI/NodeGraphCanvas.h>
#include <ibis/UI/TimelineWidget.h>
#include <ibis/UI/Viewport.h>

#include <ibis/Models/Document.h>

#include <ftk/UI/Divider.h>
#include <ftk/UI/RowLayout.h>
#include <ftk/UI/Splitter.h>
#include <ftk/UI/TabWidget.h>

namespace ibis
{
    struct DocumentWidget::Private
    {
        std::shared_ptr<ui::Viewport> viewport;
        std::shared_ptr<ui::NodeGraphCanvas> nodeGraphCanvas;
        std::shared_ptr<ui::TimelineWidget> timelineWidget;
        std::shared_ptr<ui::NodeBrowser> nodeBrowser;
        std::shared_ptr<ui::NodeEditor> nodeEditor;
        std::shared_ptr<ftk::Splitter> splitterH;
        std::shared_ptr<ftk::Splitter> splitterV;
        std::shared_ptr<ftk::TabWidget> tabWidget;
        std::shared_ptr<ftk::VerticalLayout> layout;
    };

    void DocumentWidget::_init(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<models::Document>& document)
    {
        ftk::IWidget::_init(context, "DocumentWidget", nullptr);
        FTK_P();

        p.viewport = ui::Viewport::create(context, document);

        p.nodeGraphCanvas = ui::NodeGraphCanvas::create(
            context,
            document,
            app->getNodeFactory());

        p.timelineWidget = ui::TimelineWidget::create(
            context,
            app->getTimeUnitsModel(),
            document->getTimeModel());

        p.nodeBrowser = ui::NodeBrowser::create(
            context,
            app->getNodeFactory());

        p.nodeEditor = ui::NodeEditor::create(
            context,
            app->getNodeWidgetFactory(),
            document);

        p.layout = ftk::VerticalLayout::create(context, shared_from_this());
        p.layout->setSpacingRole(ftk::SizeRole::None);
        p.splitterH = ftk::Splitter::create(context, ftk::Orientation::Horizontal, p.layout);
        p.splitterH->setSplit(.8F);
        auto vLayout = ftk::VerticalLayout::create(context, p.splitterH);
        vLayout->setSpacingRole(ftk::SizeRole::None);
        p.splitterV = ftk::Splitter::create(context, ftk::Orientation::Vertical, vLayout);
        p.splitterV->setSplit(.6F);
        p.viewport->setParent(p.splitterV);
        p.nodeGraphCanvas->setParent(p.splitterV);
        ftk::Divider::create(context, ftk::Orientation::Vertical, vLayout);
        p.timelineWidget->setParent(vLayout);
        p.tabWidget = ftk::TabWidget::create(context, p.splitterH);
        p.tabWidget->addTab("Node Browser", p.nodeBrowser);
        p.tabWidget->addTab("Node Editor", p.nodeEditor);
    }

    DocumentWidget::DocumentWidget() :
        _p(new Private)
    {}

    DocumentWidget::~DocumentWidget()
    {}

    std::shared_ptr<DocumentWidget> DocumentWidget::create(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<models::Document>& document)
    {
        auto out = std::shared_ptr<DocumentWidget>(new DocumentWidget);
        out->_init(context, app, document);
        return out;
    }

    std::shared_ptr<ui::Viewport> DocumentWidget::getViewport() const
    {
        return _p->viewport;
    }

    ftk::Size2I DocumentWidget::getSizeHint() const
    {
        return _p->layout->getSizeHint();
    }

    void DocumentWidget::setGeometry(const ftk::Box2I& value)
    {
        IWidget::setGeometry(value);
        _p->layout->setGeometry(value);
    }
}
