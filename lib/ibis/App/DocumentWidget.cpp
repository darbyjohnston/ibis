// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include "DocumentWidget.h"

#include "App.h"

#include <ibis/UI/TimelineWidget.h>
#include <ibis/UI/NodeGraphCanvas.h>
#include <ibis/UI/NodeMiniMap.h>
#include <ibis/UI/Viewport.h>

#include <ibis/Models/Document.h>

#include <ftk/UI/Divider.h>
#include <ftk/UI/RowLayout.h>
#include <ftk/UI/ScrollWidget.h>
#include <ftk/UI/Splitter.h>

namespace ibis
{
    struct DocumentWidget::Private
    {
        std::shared_ptr<ui::Viewport> viewport;
        std::shared_ptr<ui::NodeGraphCanvas> canvas;
        std::shared_ptr<ui::NodeMiniMap> miniMap;
        std::shared_ptr<ftk::ScrollWidget> canvasScrollWidget;
        std::shared_ptr<ui::TimelineWidget> timelineWidget;
        std::shared_ptr<ftk::Splitter> splitter;
        std::shared_ptr<ftk::VerticalLayout> layout;
    };

    void DocumentWidget::_init(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<models::Document>& document,
        const std::map<std::string, std::shared_ptr<ftk::Action> >& editActions)
    {
        ftk::IWidget::_init(context, "DocumentWidget", nullptr);
        FTK_P();

        p.viewport = ui::Viewport::create(context, document);

        p.canvas = ui::NodeGraphCanvas::create(
            context,
            app->getNodeFactory(),
            document,
            editActions);

        p.timelineWidget = ui::TimelineWidget::create(
            context,
            app->getTimeUnitsModel(),
            document->getTimeModel());

        p.miniMap = ui::NodeMiniMap::create(context);

        p.layout = ftk::VerticalLayout::create(context, shared_from_this());
        p.layout->setSpacingRole(ftk::SizeRole::None);
        p.splitter = ftk::Splitter::create(context, ftk::Orientation::Vertical, p.layout);
        p.splitter->setSplit(.6F);
        p.viewport->setParent(p.splitter);
        p.canvasScrollWidget = ftk::ScrollWidget::create(context, ftk::ScrollType::Both, p.splitter);
        p.canvasScrollWidget->setBorder(false);
        p.canvasScrollWidget->setWidget(p.canvas);
        p.canvasScrollWidget->setViewportWidget(p.miniMap);
        ftk::Divider::create(context, ftk::Orientation::Vertical, p.layout);
        p.timelineWidget->setParent(p.layout);

        p.canvasScrollWidget->setScrollInfoCallback(
            [this](const ftk::ScrollInfo& value)
            {
                _p->miniMap->setScrollInfo(value);
            });

        p.canvas->setChildGeometryCallback(
            [this](const std::vector<ftk::Box2I>& value)
            {
                _p->miniMap->setChildGeometry(value);
            });

        p.miniMap->setCallback(
            [this](const ftk::V2I& value)
            {
                _p->canvasScrollWidget->setScrollPos(value);
            });
    }

    DocumentWidget::DocumentWidget() :
        _p(new Private)
    {}

    DocumentWidget::~DocumentWidget()
    {}

    std::shared_ptr<DocumentWidget> DocumentWidget::create(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<models::Document>& document,
        const std::map<std::string, std::shared_ptr<ftk::Action> >& editActions)
    {
        auto out = std::shared_ptr<DocumentWidget>(new DocumentWidget);
        out->_init(context, app, document, editActions);
        return out;
    }

    std::shared_ptr<ui::Viewport> DocumentWidget::getViewport() const
    {
        return _p->viewport;
    }

    ftk::Box2I DocumentWidget::getCanvasViewportRect() const
    {
        FTK_P();
        auto scrollArea = p.canvasScrollWidget->getScrollArea();
        return scrollArea->getGeometry();
    }

    ftk::Box2I DocumentWidget::getCanvasViewRect() const
    {
        FTK_P();
        auto scrollArea = p.canvasScrollWidget->getScrollArea();
        return ftk::Box2I(scrollArea->getScrollPos(), scrollArea->getGeometry().size());
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
