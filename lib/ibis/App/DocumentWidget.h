// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#pragma once

#include <ibis/UI/NodeBrowser.h>
#include <ibis/UI/NodeEditor.h>
#include <ibis/UI/NodeGraphCanvas.h>
#include <ibis/UI/TimelineWidget.h>
#include <ibis/UI/Viewport.h>

#include <ibis/Models/Document.h>

#include <ftk/UI/IWidget.h>
#include <ftk/UI/RowLayout.h>
#include <ftk/UI/Splitter.h>
#include <ftk/UI/TabWidget.h>

namespace ibis
{
    class App;

    //! Document widget.
    class DocumentWidget : public ftk::IWidget
    {
        FTK_NON_COPYABLE(DocumentWidget);

    protected:
        void _init(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<models::Document>&);

        DocumentWidget() = default;

    public:
        ~DocumentWidget();

        static std::shared_ptr<DocumentWidget> create(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<models::Document>&);

        ftk::Size2I getSizeHint() const override;
        void setGeometry(const ftk::Box2I&) override;

    private:
        std::shared_ptr<ui::Viewport> _viewport;
        std::shared_ptr<ui::NodeGraphCanvas> _nodeGraphCanvas;
        std::shared_ptr<ui::TimelineWidget> _timelineWidget;
        std::shared_ptr<ui::NodeBrowser> _nodeBrowser;
        std::shared_ptr<ui::NodeEditor> _nodeEditor;
        std::shared_ptr<ftk::Splitter> _splitterH;
        std::shared_ptr<ftk::Splitter> _splitterV;
        std::shared_ptr<ftk::TabWidget> _tabWidget;
        std::shared_ptr<ftk::VerticalLayout> _layout;
    };
}