// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#pragma once

#include <ftk/UI/Action.h>
#include <ftk/UI/IWidget.h>

namespace ibis
{
    namespace models
    {
        class Document;
    }

    namespace ui
    {
        class Viewport;
    }

    class App;

    //! Document widget.
    class DocumentWidget : public ftk::IWidget
    {
        FTK_NON_COPYABLE(DocumentWidget);

    protected:
        void _init(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<models::Document>&,
            const std::map<std::string, std::shared_ptr<ftk::Action> >& editActions);

        DocumentWidget();

    public:
        ~DocumentWidget();

        static std::shared_ptr<DocumentWidget> create(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<models::Document>&,
            const std::map<std::string, std::shared_ptr<ftk::Action> >& editActions);

        std::shared_ptr<ui::Viewport> getViewport() const;

        ftk::Box2I getCanvasViewRect() const;

        ftk::Size2I getSizeHint() const override;
        void setGeometry(const ftk::Box2I&) override;

    private:
        FTK_PRIVATE();
    };
}