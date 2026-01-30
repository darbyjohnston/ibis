// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include "DocumentWidget.h"

#include "App.h"

#include <ibis/UI/TimelineWidget.h>
#include <ibis/UI/NodeGraphCanvas.h>
#include <ibis/UI/Viewport.h>

#include <ibis/Models/Document.h>

#include <ftk/UI/Divider.h>
#include <ftk/UI/RowLayout.h>
#include <ftk/UI/Splitter.h>

namespace ibis
{
    struct DocumentWidget::Private
    {
        std::shared_ptr<ui::Viewport> viewport;
        std::shared_ptr<ui::NodeGraphCanvas> nodeGraphCanvas;
        std::shared_ptr<ui::TimelineWidget> timelineWidget;
        std::shared_ptr<ftk::Splitter> splitter;
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

        p.layout = ftk::VerticalLayout::create(context, shared_from_this());
        p.layout->setSpacingRole(ftk::SizeRole::None);
        p.splitter = ftk::Splitter::create(context, ftk::Orientation::Vertical, p.layout);
        p.splitter->setSplit(.6F);
        p.viewport->setParent(p.splitter);
        p.nodeGraphCanvas->setParent(p.splitter);
        ftk::Divider::create(context, ftk::Orientation::Vertical, p.layout);
        p.timelineWidget->setParent(p.layout);
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
