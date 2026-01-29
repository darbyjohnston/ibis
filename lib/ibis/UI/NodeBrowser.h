// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ibis/UI/NodeDragDrop.h>

#include <ibis/Render/NodeFactory.h>

#include <ftk/UI/IMouseWidget.h>

namespace ibis
{
    namespace ui
    {
        //! Node browser item.
        class NodeBrowserItem : public ftk::IMouseWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const render::NodeInfo&,
                const std::shared_ptr<ftk::IWidget>& parent);

            NodeBrowserItem();

        public:
            virtual ~NodeBrowserItem();

            //! Create a new widget.
            static std::shared_ptr<NodeBrowserItem> create(
                const std::shared_ptr<ftk::Context>&,
                const render::NodeInfo&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;
            void sizeHintEvent(const ftk::SizeHintEvent&) override;
            void drawEvent(const ftk::Box2I&, const ftk::DrawEvent&) override;
            void mouseEnterEvent(ftk::MouseEnterEvent&) override;
            void mouseLeaveEvent() override;
            void mouseMoveEvent(ftk::MouseMoveEvent&) override;

        private:
            FTK_PRIVATE();
        };

        //! Node browser.
        class NodeBrowser : public ftk::IWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<render::NodeFactory>&,
                const std::shared_ptr<ftk::IWidget>& parent);

            NodeBrowser();

        public:
            virtual ~NodeBrowser();

            //! Create a new widget.
            static std::shared_ptr<NodeBrowser> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<render::NodeFactory>&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;

        private:
            FTK_PRIVATE();
        };
    }
}
