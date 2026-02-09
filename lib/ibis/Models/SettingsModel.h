// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#pragma once

#include <ftk/UI/App.h>
#include <ftk/UI/FileBrowser.h>
#include <ftk/UI/Settings.h>

namespace ibis
{
    namespace models
    {
        //! Canvas settings.
        struct CanvasSettings
        {
            bool miniMap = true;

            bool operator == (const CanvasSettings&) const;
            bool operator != (const CanvasSettings&) const;
        };

        //! File browser settings.
        struct FileBrowserSettings
        {
            bool                    nativeFileDialog = true;
            ftk::FileBrowserOptions options;

            bool operator == (const FileBrowserSettings&) const;
            bool operator != (const FileBrowserSettings&) const;
        };

        //! Style settings.
        struct StyleSettings
        {
            float              displayScale  = 1.F;
            ftk::ColorControls colorControls;
            ftk::ColorStyle    colorStyle    = ftk::ColorStyle::Dark;

            bool operator == (const StyleSettings&) const;
            bool operator != (const StyleSettings&) const;
        };

        //! This model provides settings that are saved and restored.
        class SettingsModel : public ftk::Settings
        {
            FTK_NON_COPYABLE(SettingsModel);

        protected:
            SettingsModel(
                const std::shared_ptr<ftk::Context>&,
                const std::filesystem::path&,
                float defaultDisplayScale);

        public:
            ~SettingsModel();

            //! Create a new model.
            static std::shared_ptr<SettingsModel> create(
                const std::shared_ptr<ftk::Context>&,
                const std::filesystem::path&,
                float defaultDisplayScale);

            //! Reset the settings.
            void reset();

            //! \name File Browser
            ///@{

            const CanvasSettings& getCanvas() const;
            std::shared_ptr<ftk::IObservable<CanvasSettings> > observeCanvas() const;
            void setCanvas(const CanvasSettings&);

            ///@}

            //! \name File Browser
            ///@{

            const FileBrowserSettings& getFileBrowser() const;
            std::shared_ptr<ftk::IObservable<FileBrowserSettings> > observeFileBrowser() const;
            void setFileBrowser(const FileBrowserSettings&);

            ///@}

            //! \name Style
            ///@{

            const StyleSettings& getStyle() const;
            std::shared_ptr<ftk::IObservable<StyleSettings> > observeStyle() const;
            void setStyle(const StyleSettings&);

            ///@}

        private:
            FTK_PRIVATE();
        };

        //! \name Serialize
        ///@{

        void to_json(nlohmann::json&, const CanvasSettings&);
        void to_json(nlohmann::json&, const FileBrowserSettings&);
        void to_json(nlohmann::json&, const StyleSettings&);

        void from_json(const nlohmann::json&, CanvasSettings&);
        void from_json(const nlohmann::json&, FileBrowserSettings&);
        void from_json(const nlohmann::json&, StyleSettings&);

        ///@}
    }
}
