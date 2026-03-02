// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "SettingsWidget.h"

#include <ftk/UI/Bellows.h>
#include <ftk/UI/CheckBox.h>
#include <ftk/UI/ComboBox.h>
#include <ftk/UI/FloatEditSlider.h>
#include <ftk/UI/FormLayout.h>
#include <ftk/UI/IntEdit.h>
#include <ftk/UI/RowLayout.h>
#include <ftk/UI/ScrollWidget.h>
#include <ftk/Core/Format.h>

namespace ibis
{
    namespace ui
    {
        struct CanvasSettingsWidget::Private
        {
            std::shared_ptr<models::SettingsModel> model;

            std::shared_ptr<ftk::CheckBox> miniMapCheckBox;
            std::shared_ptr<ftk::FormLayout> layout;

            std::shared_ptr<ftk::Observer<models::CanvasSettings> > settingsObserver;
        };

        void CanvasSettingsWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::SettingsModel>& model,
            const std::shared_ptr<IWidget>& parent)
        {
            IWidget::_init(context, "ibis::ui::CanvasSettingsWidget", parent);
            FTK_P();

            p.model = model;

            p.miniMapCheckBox = ftk::CheckBox::create(context);
            p.miniMapCheckBox->setHStretch(ftk::Stretch::Expanding);

            p.layout = ftk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(ftk::SizeRole::Margin);
            p.layout->setSpacingRole(ftk::SizeRole::SpacingSmall);
            p.layout->addRow("Mini map:", p.miniMapCheckBox);
            
            p.settingsObserver = ftk::Observer<models::CanvasSettings>::create(
                p.model->observeCanvas(),
                [this](const models::CanvasSettings& value)
                {
                    FTK_P();
                    p.miniMapCheckBox->setChecked(value.miniMap);
                });

            p.miniMapCheckBox->setCheckedCallback(
                [this](bool value)
                {
                    FTK_P();
                    auto settings = p.model->getCanvas();
                    settings.miniMap = value;
                    p.model->setCanvas(settings);
                });
        }

        CanvasSettingsWidget::CanvasSettingsWidget() :
            _p(new Private)
        {}

        CanvasSettingsWidget::~CanvasSettingsWidget()
        {}

        std::shared_ptr<CanvasSettingsWidget> CanvasSettingsWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::SettingsModel>& model,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<CanvasSettingsWidget>(new CanvasSettingsWidget);
            out->_init(context, model, parent);
            return out;
        }

        ftk::Size2I CanvasSettingsWidget::getSizeHint() const
        {
            return _p->layout->getSizeHint();
        }

        void CanvasSettingsWidget::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

#if defined(FTK_NFD)
        struct FileBrowserSettingsWidget::Private
        {
            std::shared_ptr<models::SettingsModel> model;

            std::shared_ptr<ftk::CheckBox> nfdCheckBox;
            std::shared_ptr<ftk::FormLayout> layout;

            std::shared_ptr<ftk::Observer<models::FileBrowserSettings> > settingsObserver;
        };

        void FileBrowserSettingsWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::SettingsModel>& model,
            const std::shared_ptr<IWidget>& parent)
        {
            IWidget::_init(context, "ibis::ui::FileBrowserSettingsWidget", parent);
            FTK_P();

            p.model = model;

            p.nfdCheckBox = ftk::CheckBox::create(context);
            p.nfdCheckBox->setHStretch(ftk::Stretch::Expanding);

            p.layout = ftk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(ftk::SizeRole::Margin);
            p.layout->setSpacingRole(ftk::SizeRole::SpacingSmall);
            p.layout->addRow("Native file dialog:", p.nfdCheckBox);

            p.settingsObserver = ftk::Observer<models::FileBrowserSettings>::create(
                p.model->observeFileBrowser(),
                [this](const models::FileBrowserSettings& value)
                {
                    FTK_P();
                    p.nfdCheckBox->setChecked(value.nativeFileDialog);
                });

            p.nfdCheckBox->setCheckedCallback(
                [this](bool value)
                {
                    FTK_P();
                    auto settings = p.model->getFileBrowser();
                    settings.nativeFileDialog = value;
                    p.model->setFileBrowser(settings);
                });
        }

        FileBrowserSettingsWidget::FileBrowserSettingsWidget() :
            _p(new Private)
        {}

        FileBrowserSettingsWidget::~FileBrowserSettingsWidget()
        {}

        std::shared_ptr<FileBrowserSettingsWidget> FileBrowserSettingsWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::SettingsModel>& model,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<FileBrowserSettingsWidget>(new FileBrowserSettingsWidget);
            out->_init(context, model, parent);
            return out;
        }

        ftk::Size2I FileBrowserSettingsWidget::getSizeHint() const
        {
            return _p->layout->getSizeHint();
        }

        void FileBrowserSettingsWidget::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }
#endif // FTK_NFD

        struct StyleSettingsWidget::Private
        {
            std::shared_ptr<models::SettingsModel> model;

            const std::vector<float> displayScales = ftk::getDisplayScales();

            std::shared_ptr<ftk::ComboBox> colorStyleComboBox;
            std::shared_ptr<ftk::FloatEditSlider> brightnessSlider;
            std::shared_ptr<ftk::FloatEditSlider> contrastSlider;
            std::shared_ptr<ftk::ComboBox> displayScaleComboBox;
            std::shared_ptr<ftk::FormLayout> layout;

            std::shared_ptr<ftk::Observer<models::StyleSettings> > settingsObserver;
        };

        void StyleSettingsWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::SettingsModel>& model,
            const std::shared_ptr<IWidget>& parent)
        {
            IWidget::_init(context, "ibis::ui::StyleSettingsWidget", parent);
            FTK_P();

            p.model = model;

            p.colorStyleComboBox = ftk::ComboBox::create(context, ftk::getColorStyleLabels());
            p.colorStyleComboBox->setHStretch(ftk::Stretch::Expanding);

            p.brightnessSlider = ftk::FloatEditSlider::create(context);
            p.brightnessSlider->setRange(.5F, 1.5F);
            p.brightnessSlider->setDefault(1.F);

            p.contrastSlider = ftk::FloatEditSlider::create(context);
            p.contrastSlider->setRange(.5F, 1.5F);
            p.contrastSlider->setDefault(1.F);

            std::vector<std::string> labels;
            for (auto d : p.displayScales)
            {
                labels.push_back(ftk::Format("{0}").arg(d).operator std::string());
            }
            p.displayScaleComboBox = ftk::ComboBox::create(context, labels);
            p.displayScaleComboBox->setHStretch(ftk::Stretch::Expanding);

            p.layout = ftk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(ftk::SizeRole::Margin);
            p.layout->setSpacingRole(ftk::SizeRole::SpacingSmall);
            p.layout->addRow("Color style:", p.colorStyleComboBox);
            p.layout->addRow("Brightness:", p.brightnessSlider);
            p.layout->addRow("Contrast:", p.contrastSlider);
            p.layout->addRow("Display scale:", p.displayScaleComboBox);

            p.settingsObserver = ftk::Observer<models::StyleSettings>::create(
                model->observeStyle(),
                [this](const models::StyleSettings& value)
                {
                    _widgetUpdate(value);
                });

            p.colorStyleComboBox->setIndexCallback(
                [this](int value)
                {
                    FTK_P();
                    auto settings = p.model->getStyle();
                    settings.colorStyle = static_cast<ftk::ColorStyle>(value);
                    p.model->setStyle(settings);
                });

            p.brightnessSlider->setCallback(
                [this](float value)
                {
                    FTK_P();
                    auto settings = p.model->getStyle();
                    settings.colorControls.brightness = value;
                    p.model->setStyle(settings);
                });

            p.contrastSlider->setCallback(
                [this](float value)
                {
                    FTK_P();
                    auto settings = p.model->getStyle();
                    settings.colorControls.contrast = value;
                    p.model->setStyle(settings);
                });

            p.displayScaleComboBox->setIndexCallback(
                [this](int value)
                {
                    FTK_P();
                    auto settings = p.model->getStyle();
                    if (value >= 0 && value < p.displayScales.size())
                    {
                        settings.displayScale = p.displayScales[value];
                    }
                    p.model->setStyle(settings);
                });
        }

        StyleSettingsWidget::StyleSettingsWidget() :
            _p(new Private)
        {}

        StyleSettingsWidget::~StyleSettingsWidget()
        {}

        std::shared_ptr<StyleSettingsWidget> StyleSettingsWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::SettingsModel>& model,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<StyleSettingsWidget>(new StyleSettingsWidget);
            out->_init(context, model, parent);
            return out;
        }

        ftk::Size2I StyleSettingsWidget::getSizeHint() const
        {
            return _p->layout->getSizeHint();
        }

        void StyleSettingsWidget::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void StyleSettingsWidget::_widgetUpdate(const models::StyleSettings& value)
        {
            FTK_P();

            p.colorStyleComboBox->setCurrentIndex(static_cast<int>(value.colorStyle));

            p.brightnessSlider->setValue(value.colorControls.brightness);
            p.contrastSlider->setValue(value.colorControls.contrast);

            const auto i = std::find(
                p.displayScales.begin(),
                p.displayScales.end(),
                value.displayScale);
            p.displayScaleComboBox->setCurrentIndex(
                i != p.displayScales.end() ?
                (i - p.displayScales.begin()) :
                -1);
        }

        struct WindowSettingsWidget::Private
        {
            std::shared_ptr<models::SettingsModel> model;

            std::shared_ptr<ftk::ComboBox> bufferTypeComboBox;
            std::shared_ptr<ftk::FormLayout> layout;

            std::shared_ptr<ftk::Observer<models::WindowSettings> > settingsObserver;
        };

        void WindowSettingsWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::SettingsModel>& model,
            const std::shared_ptr<IWidget>& parent)
        {
            IWidget::_init(context, "ibis::ui::WindowSettingsWidget", parent);
            FTK_P();

            p.model = model;

            p.bufferTypeComboBox = ftk::ComboBox::create(context, ftk::getWindowBufferTypeLabels());
            p.bufferTypeComboBox->setHStretch(ftk::Stretch::Expanding);

            p.layout = ftk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(ftk::SizeRole::Margin);
            p.layout->setSpacingRole(ftk::SizeRole::SpacingSmall);
            p.layout->addRow("Buffer type:", p.bufferTypeComboBox);

            p.settingsObserver = ftk::Observer<models::WindowSettings>::create(
                model->observeWindow(),
                [this](const models::WindowSettings& value)
                {
                    _widgetUpdate(value);
                });

            p.bufferTypeComboBox->setIndexCallback(
                [this](int value)
                {
                    FTK_P();
                    auto settings = p.model->getWindow();
                    settings.bufferType = static_cast<ftk::WindowBufferType>(value);
                    p.model->setWindow(settings);
                });
        }

        WindowSettingsWidget::WindowSettingsWidget() :
            _p(new Private)
        {}

        WindowSettingsWidget::~WindowSettingsWidget()
        {}

        std::shared_ptr<WindowSettingsWidget> WindowSettingsWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::SettingsModel>& model,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<WindowSettingsWidget>(new WindowSettingsWidget);
            out->_init(context, model, parent);
            return out;
        }

        ftk::Size2I WindowSettingsWidget::getSizeHint() const
        {
            return _p->layout->getSizeHint();
        }

        void WindowSettingsWidget::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void WindowSettingsWidget::_widgetUpdate(const models::WindowSettings& value)
        {
            FTK_P();

            p.bufferTypeComboBox->setCurrentIndex(static_cast<int>(value.bufferType));
        }

        struct SettingsWidget::Private
        {
            std::map<std::string, std::shared_ptr<ftk::Bellows> > bellows;
            std::shared_ptr<ftk::ScrollWidget> scrollWidget;
        };

        void SettingsWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::SettingsModel>& model,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::SettingsWidget", parent);
            FTK_P();

            auto layout = ftk::VerticalLayout::create(context, shared_from_this());
            layout->setSpacingRole(ftk::SizeRole::None);

            p.bellows["Canvas"] = ftk::Bellows::create(context, "Canvas", layout);
            p.bellows["Canvas"]->setWidget(CanvasSettingsWidget::create(context, model));
#if defined(FTK_NFD)
            p.bellows["File Browser"] = ftk::Bellows::create(context, "File Browser", layout);
            p.bellows["File Browser"]->setWidget(FileBrowserSettingsWidget::create(context, model));
#endif // FTK_NFD
            p.bellows["Style"] = ftk::Bellows::create(context, "Style", layout);
            p.bellows["Style"]->setWidget(StyleSettingsWidget::create(context, model));
            p.bellows["Window"] = ftk::Bellows::create(context, "Window", layout);
            p.bellows["Window"]->setWidget(WindowSettingsWidget::create(context, model));

            p.scrollWidget = ftk::ScrollWidget::create(context, ftk::ScrollType::Both, shared_from_this());
            p.scrollWidget->setBorder(false);
            p.scrollWidget->setWidget(layout);
        }

        SettingsWidget::SettingsWidget() :
            _p(new Private)
        {}

        SettingsWidget::~SettingsWidget()
        {}

        std::shared_ptr<SettingsWidget> SettingsWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::SettingsModel>& model,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<SettingsWidget> out(new SettingsWidget);
            out->_init(context, model, parent);
            return out;
        }

        ftk::Size2I SettingsWidget::getSizeHint() const
        {
            return _p->scrollWidget->getSizeHint();
        }

        void SettingsWidget::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->scrollWidget->setGeometry(value);
        }
    }
}
