// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ftk/UI/IWidget.h>

namespace ibis
{
    namespace models
    {
        class Document;
    }

    namespace render
    {
        class NodeFactory;
    }

    namespace ui
    {
        //! Node graph canvas.
        class NodeGraphCanvas : public ftk::IWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::Document>&,
                const std::shared_ptr<render::NodeFactory>&,
                const std::shared_ptr<ftk::IWidget>& parent);

            NodeGraphCanvas();

        public:
            virtual ~NodeGraphCanvas();

            //! Create a new widget.
            static std::shared_ptr<NodeGraphCanvas> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::Document>&,
                const std::shared_ptr<render::NodeFactory>&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;
            void sizeHintEvent(const ftk::SizeHintEvent&) override;
            void drawOverlayEvent(const ftk::Box2I&, const ftk::DrawEvent&) override;
            void dragEnterEvent(ftk::DragDropEvent&) override;
            void dragLeaveEvent(ftk::DragDropEvent&) override;
            void dragMoveEvent(ftk::DragDropEvent&) override;
            void dropEvent(ftk::DragDropEvent&) override;

        private:
            void _graphUpdate();

            FTK_PRIVATE();
        };
    }
}
