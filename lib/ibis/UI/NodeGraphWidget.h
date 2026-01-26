// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ftk/UI/IWidget.h>

namespace ibis
{
    namespace render
    {
        class INode;
    }

    namespace ui
    {
        //! Node graph input.
        class NodeGraphInput : public ftk::IWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent);

            NodeGraphInput();

        public:
            virtual ~NodeGraphInput();

            //! Create a new widget.
            static std::shared_ptr<NodeGraphInput> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;
            void sizeHintEvent(const ftk::SizeHintEvent&) override;

        private:
            FTK_PRIVATE();
        };

        //! Node graph output.
        class NodeGraphOutput : public ftk::IWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent);

            NodeGraphOutput();

        public:
            virtual ~NodeGraphOutput();

            //! Create a new widget.
            static std::shared_ptr<NodeGraphOutput> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;
            void sizeHintEvent(const ftk::SizeHintEvent&) override;

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
                const std::shared_ptr<ftk::IWidget>& parent);

            NodeGraphWidget();

        public:
            virtual ~NodeGraphWidget();

            //! Create a new widget.
            static std::shared_ptr<NodeGraphWidget> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);

            const std::shared_ptr<render::INode>& getNode() const;

            const std::vector<std::shared_ptr<NodeGraphInput> >& getInputs() const;
            const std::vector<std::shared_ptr<NodeGraphOutput> >& getOutputs() const;

            bool isSelected() const;
            void setSelected(bool);

            bool isView() const;
            void setView(bool);
            void setViewCallback(const std::function<void(const std::shared_ptr<render::INode>&)>&);

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;

        private:
            FTK_PRIVATE();
        };
    }
}
