// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ftk/UI/Action.h>
#include <ftk/UI/IWidget.h>

#include <optional>

namespace ibis
{
    namespace models
    {
        class Document;
    }

    namespace render
    {
        class INode;
        class NodeFactory;
    }

    namespace ui
    {
        class INodePort;
        class NodeGraphWidget;

        //! Node graph canvas.
        class NodeGraphCanvas : public ftk::IWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<render::NodeFactory>&,
                const std::shared_ptr<models::Document>&,
                const std::map<std::string, std::shared_ptr<ftk::Action> >& editActions,
                const std::shared_ptr<ftk::IWidget>& parent);

            NodeGraphCanvas();

        public:
            virtual ~NodeGraphCanvas();

            //! Create a new widget.
            static std::shared_ptr<NodeGraphCanvas> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<render::NodeFactory>&,
                const std::shared_ptr<models::Document>&,
                const std::map<std::string, std::shared_ptr<ftk::Action> >& editActions,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);

            void scrollTo(const std::shared_ptr<render::INode>&);

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;
            void sizeHintEvent(const ftk::SizeHintEvent&) override;
            void drawOverlayEvent(const ftk::Box2I&, const ftk::DrawEvent&) override;
            void drawEvent(const ftk::Box2I&, const ftk::DrawEvent&) override;
            void mouseEnterEvent(ftk::MouseEnterEvent&) override;
            void mouseLeaveEvent() override;
            void mouseMoveEvent(ftk::MouseMoveEvent&) override;
            void mousePressEvent(ftk::MouseClickEvent&) override;
            void mouseReleaseEvent(ftk::MouseClickEvent&) override;
            void dragEnterEvent(ftk::DragDropEvent&) override;
            void dragLeaveEvent(ftk::DragDropEvent&) override;
            void dragMoveEvent(ftk::DragDropEvent&) override;
            void dropEvent(ftk::DragDropEvent&) override;

        private:
            struct Move
            {
                std::shared_ptr<render::INode> node;
                std::shared_ptr<NodeGraphWidget> widget;
            };
            std::optional<Move> _getMove(const ftk::MouseClickEvent&);

            struct Connect
            {
                std::shared_ptr<render::INode> node;
                std::shared_ptr<NodeGraphWidget> widget;
                int input = -1;
                int output = -1;
            };
            std::optional<Connect> _getConnect(const ftk::MouseClickEvent&);

            struct Input
            {
                std::shared_ptr<render::INode> node;
                std::shared_ptr<NodeGraphWidget> widget;
                int input = -1;
            };
            std::optional<Input> _getInput(const ftk::V2I&);

            struct Output
            {
                std::shared_ptr<render::INode> node;
                std::shared_ptr<NodeGraphWidget> widget;
                int output = -1;
            };
            std::optional<Output> _getOutput(const ftk::V2I&);

            ftk::Box2I _getSelectionRect() const;
            std::shared_ptr<render::INode> _getNode(const ftk::V2I&) const;
            std::vector<std::shared_ptr<render::INode> > _getNodes(const ftk::Box2I&) const;

            void _popupMenu(const ftk::V2I&);

            void _graphUpdate();
            void _autoScrollUpdate();

            FTK_PRIVATE();
        };
    }
}
