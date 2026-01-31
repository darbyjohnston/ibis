// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ibis/UI/INodeWidget.h>

namespace ibis
{
    namespace ui
    {
        //! Solid color node widget.
        class SolidColorNodeWidget : public INodeWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ibis::models::Document>&,
                const std::shared_ptr<ibis::render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent);

            SolidColorNodeWidget();

        public:
            virtual ~SolidColorNodeWidget();

            //! Create a new widget.
            static std::shared_ptr<SolidColorNodeWidget> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ibis::models::Document>&,
                const std::shared_ptr<ibis::render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);

            static render::NodeInfo getClassNodeInfo();

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;

        private:
            void _callback(
                const std::vector<std::pair<std::string, nlohmann::json> >&,
                bool pressed);

            FTK_PRIVATE();
        };

        //! Gradient node widget.
        class GradientNodeWidget : public INodeWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ibis::models::Document>&,
                const std::shared_ptr<ibis::render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent);

            GradientNodeWidget();

        public:
            virtual ~GradientNodeWidget();

            //! Create a new widget.
            static std::shared_ptr<GradientNodeWidget> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ibis::models::Document>&,
                const std::shared_ptr<ibis::render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);

            static render::NodeInfo getClassNodeInfo();

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;

        private:
            void _callback(
                const std::vector<std::pair<std::string, nlohmann::json> >&,
                bool pressed);

            FTK_PRIVATE();
        };

        //! Noise node widget.
        class NoiseNodeWidget : public INodeWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ibis::models::Document>&,
                const std::shared_ptr<ibis::render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent);

            NoiseNodeWidget();

        public:
            virtual ~NoiseNodeWidget();

            //! Create a new widget.
            static std::shared_ptr<NoiseNodeWidget> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ibis::models::Document>&,
                const std::shared_ptr<ibis::render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);

            static render::NodeInfo getClassNodeInfo();

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;

        private:
            void _callback(
                const std::vector<std::pair<std::string, nlohmann::json> >&,
                bool pressed);

            FTK_PRIVATE();
        };
    }
}
