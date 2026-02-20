// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include "SettingsModel.h"

namespace ibis
{
    namespace models
    {
        bool CanvasSettings::operator == (const CanvasSettings& other) const
        {
            return miniMap == other.miniMap;
        }

        bool CanvasSettings::operator != (const CanvasSettings& other) const
        {
            return !(*this == other);
        }

        bool FileBrowserSettings::operator == (const FileBrowserSettings& other) const
        {
            return
                nativeFileDialog == other.nativeFileDialog &&
                options == other.options;
        }

        bool FileBrowserSettings::operator != (const FileBrowserSettings& other) const
        {
            return !(*this == other);
        }

        bool StyleSettings::operator == (const StyleSettings& other) const
        {
            return
                displayScale == other.displayScale &&
                colorControls == other.colorControls &&
                colorStyle == other.colorStyle;
        }

        bool StyleSettings::operator != (const StyleSettings& other) const
        {
            return !(*this == other);
        }

        bool WindowSettings::operator == (const WindowSettings& other) const
        {
            return
                bufferType == other.bufferType;
        }

        bool WindowSettings::operator != (const WindowSettings& other) const
        {
            return !(*this == other);
        }

        struct SettingsModel::Private
        {
            std::shared_ptr<ftk::Observable<CanvasSettings> > canvas;
            std::weak_ptr<ftk::FileBrowserSystem> fileBrowserSystem;
            std::shared_ptr<ftk::Observable<FileBrowserSettings> > fileBrowser;
            float defaultDisplayScale = 1.F;
            std::shared_ptr<ftk::Observable<StyleSettings> > style;
            std::shared_ptr<ftk::Observable<WindowSettings> > window;
        };

        SettingsModel::SettingsModel(
            const std::shared_ptr<ftk::Context>& context,
            const std::filesystem::path& path,
            float defaultDisplayScale) :
            Settings(context, path, false),
            _p(new  Private)
        {
            FTK_P();

            CanvasSettings canvas;
            getT("/Canvas", canvas);
            p.canvas = ftk::Observable<CanvasSettings>::create(canvas);

            FileBrowserSettings fileBrowser;
            getT("/FileBrowser", fileBrowser);
            p.fileBrowser = ftk::Observable<FileBrowserSettings>::create(fileBrowser);
            auto fileBrowserSystem = context->getSystem<ftk::FileBrowserSystem>();
            fileBrowserSystem->setNativeFileDialog(fileBrowser.nativeFileDialog);
            fileBrowserSystem->getModel()->setOptions(fileBrowser.options);
            p.fileBrowserSystem = fileBrowserSystem;

            p.defaultDisplayScale = defaultDisplayScale;
            StyleSettings style;
            style.displayScale = defaultDisplayScale;
            getT("/Style", style);
            p.style = ftk::Observable<StyleSettings>::create(style);

            WindowSettings window;
            getT("/Window", window);
            p.window = ftk::Observable<WindowSettings>::create(window);
        }
        
        SettingsModel::~SettingsModel()
        {
            FTK_P();
            setT("/Canvas", p.canvas->get());
            if (auto fileBrowserSystem = p.fileBrowserSystem.lock())
            {
                FileBrowserSettings fileBrowser = p.fileBrowser->get();
                fileBrowser.options = fileBrowserSystem->getModel()->getOptions();
                setT("/FileBrowser", fileBrowser);
            }
            setT("/Style", p.style->get());
            setT("/Window", p.window->get());
        }

        std::shared_ptr<SettingsModel> SettingsModel::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::filesystem::path& path,
            float defaultDisplayScale)
        {
            return std::shared_ptr<SettingsModel>(new SettingsModel(context, path, defaultDisplayScale));
        }

        void SettingsModel::reset()
        {
            FTK_P();
            setCanvas(CanvasSettings());
            setFileBrowser(FileBrowserSettings());
            StyleSettings style;
            style.displayScale = p.defaultDisplayScale;
            setStyle(style);
            setWindow(WindowSettings());
        }

        const CanvasSettings& SettingsModel::getCanvas() const
        {
            return _p->canvas->get();
        }

        std::shared_ptr<ftk::IObservable<CanvasSettings> > SettingsModel::observeCanvas() const
        {
            return _p->canvas;
        }

        void SettingsModel::setCanvas(const CanvasSettings& value)
        {
            _p->canvas->setIfChanged(value);
        }

        const FileBrowserSettings& SettingsModel::getFileBrowser() const
        {
            return _p->fileBrowser->get();
        }

        std::shared_ptr<ftk::IObservable<FileBrowserSettings> > SettingsModel::observeFileBrowser() const
        {
            return _p->fileBrowser;
        }

        void SettingsModel::setFileBrowser(const FileBrowserSettings& value)
        {
            FTK_P();
            if (p.fileBrowser->setIfChanged(value))
            {
                if (auto fileBrowserSystem = p.fileBrowserSystem.lock())
                {
                    fileBrowserSystem->setNativeFileDialog(value.nativeFileDialog);
                }
            }
        }

        const StyleSettings& SettingsModel::getStyle() const
        {
            return _p->style->get();
        }

        std::shared_ptr<ftk::IObservable<StyleSettings> > SettingsModel::observeStyle() const
        {
            return _p->style;
        }

        void SettingsModel::setStyle(const StyleSettings& value)
        {
            _p->style->setIfChanged(value);
        }

        const WindowSettings& SettingsModel::getWindow() const
        {
            return _p->window->get();
        }

        std::shared_ptr<ftk::IObservable<WindowSettings> > SettingsModel::observeWindow() const
        {
            return _p->window;
        }

        void SettingsModel::setWindow(const WindowSettings& value)
        {
            _p->window->setIfChanged(value);
        }

        void to_json(nlohmann::json& json, const CanvasSettings& value)
        {
            json["MiniMap"] = value.miniMap;
        }

        void to_json(nlohmann::json& json, const FileBrowserSettings& value)
        {
            json["NativeFileDialog"] = value.nativeFileDialog;
            json["Options"] = value.options;
        }

        void to_json(nlohmann::json& json, const StyleSettings& value)
        {
            json["DisplayScale"] = value.displayScale;
            json["ColorControls"] = value.colorControls;
            json["ColorStyle"] = to_string(value.colorStyle);
        }

        void to_json(nlohmann::json& json, const WindowSettings& value)
        {
            json["BufferType"] = to_string(value.bufferType);
        }

        void from_json(const nlohmann::json& json, CanvasSettings& value)
        {
            json.at("MiniMap").get_to(value.miniMap);
        }

        void from_json(const nlohmann::json& json, FileBrowserSettings& value)
        {
            json.at("NativeFileDialog").get_to(value.nativeFileDialog);
            json.at("Options").get_to(value.options);
        }

        void from_json(const nlohmann::json& json, StyleSettings& value)
        {
            json.at("DisplayScale").get_to(value.displayScale);
            from_string(json.at("ColorStyle").get<std::string>(), value.colorStyle);
            json.at("ColorControls").get_to(value.colorControls);
        }

        void from_json(const nlohmann::json& json, WindowSettings& value)
        {
            from_string(json.at("BufferType").get<std::string>(), value.bufferType);
        }
    }
}
