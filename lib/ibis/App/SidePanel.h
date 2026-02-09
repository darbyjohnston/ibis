// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ibis/UI/NodeDragDrop.h>

#include <ibis/Render/NodeFactory.h>

#include <ftk/UI/IMouseWidget.h>

namespace ibis
{
    class App;

    //! Side panels.
    enum class SidePanel
    {
        NodeBrowser,
        NodeEditor,
        Document,
        Settings,
        Messages,
        Diag,
        SysLog,

        Count,
        First = NodeBrowser
    };
    FTK_ENUM(SidePanel);

    //! Side panel widget.
    class SidePanelWidget : public ftk::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<ftk::IWidget>& parent);

        SidePanelWidget();

    public:
        virtual ~SidePanelWidget();

        //! Create a new widget.
        static std::shared_ptr<SidePanelWidget> create(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<ftk::IWidget>& parent = nullptr);

        void setSidePanel(SidePanel);
        void setCallback(const std::function<void(SidePanel)>&);

        ftk::Size2I getSizeHint() const override;
        void setGeometry(const ftk::Box2I&) override;

    private:
        void _widgetUpdate(SidePanel);

        FTK_PRIVATE();
    };
}

