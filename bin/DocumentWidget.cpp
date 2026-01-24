// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include "DocumentWidget.h"

#include "App.h"

#include <ftk/UI/Divider.h>

namespace ibis
{
    void DocumentWidget::_init(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<models::Document>& document)
    {
        ftk::IWidget::_init(context, "DocumentWidget", nullptr);

        _viewport = ui::Viewport::create(context, document->getGraph());

        _nodeGraphCanvas = ui::NodeGraphCanvas::create(context, document, app->getNodeFactory());

        _timelineWidget = ui::TimelineWidget::create(context);

        _nodeBrowser = ui::NodeBrowser::create(context, app->getNodeFactory());

        _nodeEditor = ui::NodeEditor::create(context, app->getNodeWidgetFactory(), document);

        _layout = ftk::VerticalLayout::create(context, shared_from_this());
        _layout->setSpacingRole(ftk::SizeRole::None);
        _splitterH = ftk::Splitter::create(context, ftk::Orientation::Horizontal, _layout);
        _splitterH->setSplit(.7F);
        auto vLayout = ftk::VerticalLayout::create(context, _splitterH);
        vLayout->setSpacingRole(ftk::SizeRole::None);
        _splitterV = ftk::Splitter::create(context, ftk::Orientation::Vertical, vLayout);
        _splitterV->setSplit(.7F);
        _viewport->setParent(_splitterV);
        _nodeGraphCanvas->setParent(_splitterV);
        ftk::Divider::create(context, ftk::Orientation::Vertical, vLayout);
        _timelineWidget->setParent(vLayout);
        _tabWidget = ftk::TabWidget::create(context, _splitterH);
        _tabWidget->addTab("Node Browser", _nodeBrowser);
        _tabWidget->addTab("Node Editor", _nodeEditor);
    }

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

    ftk::Size2I DocumentWidget::getSizeHint() const
    {
        return _layout->getSizeHint();
    }

    void DocumentWidget::setGeometry(const ftk::Box2I& value)
    {
        IWidget::setGeometry(value);
        _layout->setGeometry(value);
    }
}