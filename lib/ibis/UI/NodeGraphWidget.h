// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ftk/UI/IMouseWidget.h>
#include <ftk/UI/IWidgetPopup.h>

namespace ibis
{
    namespace render
    {
        class INode;
    }

    namespace models
    {
        class Document;
    }

    namespace ui
    {
        class NodeWidgetFactory;

        //! Base class for node graph port widgets.
        class INodeGraphPort : public ftk::IMouseWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent);

            INodeGraphPort();

        public:
            virtual ~INodeGraphPort() = 0;

            void setConnect(bool);

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;
            void sizeHintEvent(const ftk::SizeHintEvent&) override;
            void drawEvent(const ftk::Box2I&, const ftk::DrawEvent&) override;
            void mouseEnterEvent(ftk::MouseEnterEvent&) override;
            void mouseLeaveEvent() override;

        private:
            FTK_PRIVATE();
        };

        //! Node graph input port widget.
        class NodeGraphInput : public INodeGraphPort
        {
        protected:
            NodeGraphInput() = default;

        public:
            virtual ~NodeGraphInput();

            //! Create a new widget.
            static std::shared_ptr<NodeGraphInput> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);
        };

        //! Node graph output port widget.
        class NodeGraphOutput : public INodeGraphPort
        {
        protected:
            NodeGraphOutput() = default;

        public:
            virtual ~NodeGraphOutput();

            //! Create a new widget.
            static std::shared_ptr<NodeGraphOutput> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);
        };

        //! Node graph thumbnail widget.
        class NodeGraphThumbnail : public ftk::IWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent);

            NodeGraphThumbnail();

        public:
            virtual ~NodeGraphThumbnail();

            //! Create a new widget.
            static std::shared_ptr<NodeGraphThumbnail> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;
            void sizeHintEvent(const ftk::SizeHintEvent&) override;
            void drawEvent(const ftk::Box2I&, const ftk::DrawEvent&) override;

        private:
            FTK_PRIVATE();
        };

        //! Node graph widget popup.
        class NodeGraphPopup : public ftk::IWidgetPopup
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<render::INode>&,
                const std::shared_ptr<models::Document>&,
                const std::shared_ptr<NodeWidgetFactory>&,
                const std::shared_ptr<ftk::IWidget>& parent);

            NodeGraphPopup();

        public:
            virtual ~NodeGraphPopup();

            //! Create a new widget.
            static std::shared_ptr<NodeGraphPopup> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<render::INode>&,
                const std::shared_ptr<models::Document>&,
                const std::shared_ptr<NodeWidgetFactory>&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);

        private:
            FTK_PRIVATE();
        };

        //! Node graph widget.
        class NodeGraphWidget : public ftk::IWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<render::INode>&,
                const std::shared_ptr<models::Document>&,
                const std::shared_ptr<NodeWidgetFactory>&,
                const std::shared_ptr<ftk::IWidget>& parent);

            NodeGraphWidget();

        public:
            virtual ~NodeGraphWidget();

            //! Create a new widget.
            static std::shared_ptr<NodeGraphWidget> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<render::INode>&,
                const std::shared_ptr<models::Document>&,
                const std::shared_ptr<NodeWidgetFactory>&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);

            const std::shared_ptr<render::INode>& getNode() const;

            const std::vector<std::shared_ptr<NodeGraphInput> >& getInputs() const;
            const std::vector<std::shared_ptr<NodeGraphOutput> >& getOutputs() const;

            void setSelected(bool);
            void setView(bool);

            void setViewCallback(const std::function<void(const std::shared_ptr<render::INode>&)>&);

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;
            void sizeHintEvent(const ftk::SizeHintEvent&) override;
            void drawEvent(const ftk::Box2I&, const ftk::DrawEvent&) override;
            void mousePressEvent(ftk::MouseClickEvent&) override;
            void mouseReleaseEvent(ftk::MouseClickEvent&) override;

        private:
            void _showPopup();

            FTK_PRIVATE();
        };
    }
}
