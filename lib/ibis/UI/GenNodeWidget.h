// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ibis/UI/INodeWidget.h>

namespace ibis
{
    namespace ui
    {
        //! Solid color node widget.
        class SolidColorNodeWidget : public IInteractionNodeWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::Document>&,
                const std::shared_ptr<render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent);

            SolidColorNodeWidget();

        public:
            virtual ~SolidColorNodeWidget();

            //! Create a new widget.
            static std::shared_ptr<SolidColorNodeWidget> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::Document>&,
                const std::shared_ptr<render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);

            static render::NodeInfo getClassNodeInfo();

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;

        private:
            FTK_PRIVATE();
        };

        //! Gradient node widget.
        class GradientNodeWidget : public IInteractionNodeWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::Document>&,
                const std::shared_ptr<render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent);

            GradientNodeWidget();

        public:
            virtual ~GradientNodeWidget();

            //! Create a new widget.
            static std::shared_ptr<GradientNodeWidget> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::Document>&,
                const std::shared_ptr<render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);

            static render::NodeInfo getClassNodeInfo();

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;

        private:
            FTK_PRIVATE();
        };

        //! Noise node widget.
        class NoiseNodeWidget : public IInteractionNodeWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::Document>&,
                const std::shared_ptr<render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent);

            NoiseNodeWidget();

        public:
            virtual ~NoiseNodeWidget();

            //! Create a new widget.
            static std::shared_ptr<NoiseNodeWidget> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::Document>&,
                const std::shared_ptr<render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);

            static render::NodeInfo getClassNodeInfo();

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;

        private:
            FTK_PRIVATE();
        };

        //! Text node widget.
        class TextNodeWidget : public IInteractionNodeWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::Document>&,
                const std::shared_ptr<render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent);

            TextNodeWidget();

        public:
            virtual ~TextNodeWidget();

            //! Create a new widget.
            static std::shared_ptr<TextNodeWidget> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::Document>&,
                const std::shared_ptr<render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);

            static render::NodeInfo getClassNodeInfo();

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;

        private:
            FTK_PRIVATE();
        };
    }
}
