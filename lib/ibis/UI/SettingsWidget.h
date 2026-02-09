// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ibis/UI/NodeDragDrop.h>

#include <ibis/Models/SettingsModel.h>

#include <ibis/Render/NodeFactory.h>

#include <ftk/UI/IMouseWidget.h>

namespace ibis
{
    namespace ui
    {
        //! Canvas settings widget.
        class CanvasSettingsWidget : public ftk::IWidget
        {
            FTK_NON_COPYABLE(CanvasSettingsWidget);

        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::SettingsModel>&,
                const std::shared_ptr<IWidget>& parent);

            CanvasSettingsWidget();

        public:
            virtual ~CanvasSettingsWidget();

            static std::shared_ptr<CanvasSettingsWidget> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::SettingsModel>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;

            FTK_PRIVATE();
        };

#if defined(FTK_NFD)
        //! File browser settings widget.
        class FileBrowserSettingsWidget : public ftk::IWidget
        {
            FTK_NON_COPYABLE(FileBrowserSettingsWidget);

        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::SettingsModel>&,
                const std::shared_ptr<IWidget>& parent);

            FileBrowserSettingsWidget();

        public:
            virtual ~FileBrowserSettingsWidget();

            static std::shared_ptr<FileBrowserSettingsWidget> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::SettingsModel>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;

            FTK_PRIVATE();
        };
#endif // FTK_NFD

        //! Style settings widget.
        class StyleSettingsWidget : public ftk::IWidget
        {
            FTK_NON_COPYABLE(StyleSettingsWidget);

        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::SettingsModel>&,
                const std::shared_ptr<IWidget>& parent);

            StyleSettingsWidget();

        public:
            virtual ~StyleSettingsWidget();

            static std::shared_ptr<StyleSettingsWidget> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::SettingsModel>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;

        private:
            void _widgetUpdate(const models::StyleSettings&);

            FTK_PRIVATE();
        };

        //! Settings widget.
        class SettingsWidget : public ftk::IWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::SettingsModel>&,
                const std::shared_ptr<ftk::IWidget>& parent);

            SettingsWidget();

        public:
            virtual ~SettingsWidget();

            //! Create a new widget.
            static std::shared_ptr<SettingsWidget> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::SettingsModel>&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;

        private:
            FTK_PRIVATE();
        };
    }
}
