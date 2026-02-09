// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "GenNodeWidget.h"

#include <ibis/Models/Document.h>

#include <ibis/Render/GenNode.h>
#include <ibis/Render/Graph.h>
#include <ibis/Render/GraphCmd.h>

#include <ftk/UI/Bellows.h>
#include <ftk/UI/ColorSwatch.h>
#include <ftk/UI/ComboBox.h>
#include <ftk/UI/FloatEditSlider.h>
#include <ftk/UI/FormLayout.h>
#include <ftk/UI/IntEditSlider.h>
#include <ftk/UI/RowLayout.h>
#include <ftk/UI/TextEdit.h>
#include <ftk/Core/String.h>

namespace ibis
{
    namespace ui
    {
        struct SolidColorNodeWidget::Private
        {
            std::shared_ptr<ftk::IntEditSlider> widthSlider;
            std::shared_ptr<ftk::IntEditSlider> heightSlider;
            std::shared_ptr<ftk::ComboBox> typeComboBox;
            std::shared_ptr<ftk::ColorSwatch> colorSwatch;
            std::shared_ptr<ftk::Bellows> bellows;

            std::shared_ptr<ftk::MapObserver<std::string, nlohmann::json> > observer;
        };

        void SolidColorNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IInteractionNodeWidget::_init(context, document, node, parent);
            FTK_P();

            p.widthSlider = ftk::IntEditSlider::create(context);
            p.widthSlider->setRange(1, 4096);
            p.widthSlider->setLargeStep(100);

            p.heightSlider = ftk::IntEditSlider::create(context);
            p.heightSlider->setRange(1, 4096);
            p.heightSlider->setLargeStep(100);

            p.typeComboBox = ftk::ComboBox::create(context, ftk::gl::getTextureTypeLabels());

            p.colorSwatch = ftk::ColorSwatch::create(context);
            p.colorSwatch->setEditable(true);

            auto formLayout = ftk::FormLayout::create(context);
            formLayout->setMarginRole(ftk::SizeRole::Margin);
            formLayout->addRow("Width:", p.widthSlider);
            formLayout->addRow("Height:", p.heightSlider);
            formLayout->addRow("Type:", p.typeComboBox);
            formLayout->addRow("Color:", p.colorSwatch);
            p.bellows = ftk::Bellows::create(context, getNodeInfo().name, shared_from_this());
            p.bellows->setOpen(true);
            p.bellows->setWidget(formLayout);

            p.widthSlider->setPressedCallback(
                [this](int value, bool pressed)
                {
                    ftk::Size2I size = _node->getAttr("Size");
                    size.w = value;
                    _callback({ { "Size", size } }, pressed);
                });

            p.heightSlider->setPressedCallback(
                [this](int value, bool pressed)
                {
                    ftk::Size2I size = _node->getAttr("Size");
                    size.h = value;
                    _callback({ { "Size", size } }, pressed);
                });

            p.typeComboBox->setIndexCallback(
                [this](int value)
                {
                    _document->command(render::NodeAttrCmd::create(
                        _document->getGraph(),
                        _node,
                        "Type",
                        static_cast<ftk::gl::TextureType>(value)));
                });

            p.colorSwatch->setPressedCallback(
                [this](const ftk::Color4F& value, bool pressed)
                {
                    _callback({ { "Color", value } }, pressed);
                });

            p.observer = ftk::MapObserver<std::string, nlohmann::json>::create(
                _node->observeAttr(),
                [this](const std::map<std::string, nlohmann::json>& value)
                {
                    FTK_P();
                    auto tmp = value;
                    const ftk::Size2I size = tmp["Size"];
                    p.widthSlider->setValue(size.w);
                    p.heightSlider->setValue(size.h);
                    const ftk::gl::TextureType type = tmp["Type"];
                    p.typeComboBox->setCurrentIndex(static_cast<int>(type));
                    p.colorSwatch->setColor(tmp["Color"]);
                });
        }

        SolidColorNodeWidget::SolidColorNodeWidget() :
            _p(new Private)
        {}

        SolidColorNodeWidget::~SolidColorNodeWidget()
        {}

        std::shared_ptr<SolidColorNodeWidget> SolidColorNodeWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<SolidColorNodeWidget> out(new SolidColorNodeWidget);
            out->_init(context, document, node, parent);
            return out;
        }

        render::NodeInfo SolidColorNodeWidget::getClassNodeInfo()
        {
            return render::SolidColorNode::getClassNodeInfo();
        }

        ftk::Size2I SolidColorNodeWidget::getSizeHint() const
        {
            return _p->bellows->getSizeHint();
        }

        void SolidColorNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            IInteractionNodeWidget::setGeometry(value);
            _p->bellows->setGeometry(value);
        }

        struct GradientNodeWidget::Private
        {
            std::shared_ptr<ftk::IntEditSlider> widthSlider;
            std::shared_ptr<ftk::IntEditSlider> heightSlider;
            std::shared_ptr<ftk::ComboBox> typeComboBox;
            std::shared_ptr<ftk::ColorSwatch> color0Swatch;
            std::shared_ptr<ftk::ColorSwatch> color1Swatch;
            std::shared_ptr<ftk::ComboBox> orientationComboBox;
            std::shared_ptr<ftk::Bellows> bellows;

            std::shared_ptr<ftk::MapObserver<std::string, nlohmann::json> > observer;
        };

        void GradientNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IInteractionNodeWidget::_init(context, document, node, parent);
            FTK_P();

            p.widthSlider = ftk::IntEditSlider::create(context);
            p.widthSlider->setRange(1, 4096);
            p.widthSlider->setLargeStep(100);

            p.heightSlider = ftk::IntEditSlider::create(context);
            p.heightSlider->setRange(1, 4096);
            p.heightSlider->setLargeStep(100);

            p.typeComboBox = ftk::ComboBox::create(context, ftk::gl::getTextureTypeLabels());

            p.color0Swatch = ftk::ColorSwatch::create(context);
            p.color0Swatch->setEditable(true);

            p.color1Swatch = ftk::ColorSwatch::create(context);
            p.color1Swatch->setEditable(true);

            p.orientationComboBox = ftk::ComboBox::create(context, ftk::getOrientationLabels());

            auto formLayout = ftk::FormLayout::create(context);
            formLayout->setMarginRole(ftk::SizeRole::Margin);
            formLayout->addRow("Width:", p.widthSlider);
            formLayout->addRow("Height:", p.heightSlider);
            formLayout->addRow("Type:", p.typeComboBox);
            formLayout->addRow("Color 0:", p.color0Swatch);
            formLayout->addRow("Color 1:", p.color1Swatch);
            formLayout->addRow("Orientation:", p.orientationComboBox);
            p.bellows = ftk::Bellows::create(context, getNodeInfo().name, shared_from_this());
            p.bellows->setOpen(true);
            p.bellows->setWidget(formLayout);

            p.widthSlider->setPressedCallback(
                [this](int value, bool pressed)
                {
                    ftk::Size2I size = _node->getAttr("Size");
                    size.w = value;
                    _callback({ { "Size", size } }, pressed);
                });

            p.heightSlider->setPressedCallback(
                [this](int value, bool pressed)
                {
                    ftk::Size2I size = _node->getAttr("Size");
                    size.h = value;
                    _callback({ { "Size", size } }, pressed);
                });

            p.typeComboBox->setIndexCallback(
                [this](int value)
                {
                    _document->command(render::NodeAttrCmd::create(
                        _document->getGraph(),
                        _node,
                        "Type",
                        static_cast<ftk::gl::TextureType>(value)));
                });

            p.color0Swatch->setPressedCallback(
                [this](const ftk::Color4F& value, bool pressed)
                {
                    _callback({ { "Color0", value } }, pressed);
                });

            p.color1Swatch->setPressedCallback(
                [this](const ftk::Color4F& value, bool pressed)
                {
                    _callback({ { "Color1", value } }, pressed);
                });

            p.orientationComboBox->setIndexCallback(
                [this](int value)
                {
                    _document->command(render::NodeAttrCmd::create(
                        _document->getGraph(),
                        _node,
                        "Orientation",
                        static_cast<ftk::Orientation>(value)));
                });

            p.observer = ftk::MapObserver<std::string, nlohmann::json>::create(
                _node->observeAttr(),
                [this](const std::map<std::string, nlohmann::json>& value)
                {
                    FTK_P();
                    auto tmp = value;
                    const ftk::Size2I size = tmp["Size"];
                    p.widthSlider->setValue(size.w);
                    p.heightSlider->setValue(size.h);
                    const ftk::gl::TextureType type = tmp["Type"];
                    p.typeComboBox->setCurrentIndex(static_cast<int>(type));
                    p.color0Swatch->setColor(tmp["Color0"]);
                    p.color1Swatch->setColor(tmp["Color1"]);
                    p.orientationComboBox->setCurrentIndex(static_cast<int>(tmp["Orientation"]));
                });
        }

        GradientNodeWidget::GradientNodeWidget() :
            _p(new Private)
        {}

        GradientNodeWidget::~GradientNodeWidget()
        {}

        std::shared_ptr<GradientNodeWidget> GradientNodeWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<GradientNodeWidget> out(new GradientNodeWidget);
            out->_init(context, document, node, parent);
            return out;
        }

        render::NodeInfo GradientNodeWidget::getClassNodeInfo()
        {
            return render::GradientNode::getClassNodeInfo();
        }

        ftk::Size2I GradientNodeWidget::getSizeHint() const
        {
            return _p->bellows->getSizeHint();
        }

        void GradientNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            IInteractionNodeWidget::setGeometry(value);
            _p->bellows->setGeometry(value);
        }

        struct NoiseNodeWidget::Private
        {
            std::shared_ptr<ftk::IntEditSlider> widthSlider;
            std::shared_ptr<ftk::IntEditSlider> heightSlider;
            std::shared_ptr<ftk::ComboBox> typeComboBox;
            std::shared_ptr<ftk::FloatEditSlider> scaleSlider;
            std::shared_ptr<ftk::Bellows> bellows;

            std::shared_ptr<ftk::MapObserver<std::string, nlohmann::json> > observer;
        };

        void NoiseNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IInteractionNodeWidget::_init(context, document, node, parent);
            FTK_P();

            p.widthSlider = ftk::IntEditSlider::create(context);
            p.widthSlider->setRange(1, 4096);
            p.widthSlider->setLargeStep(100);

            p.heightSlider = ftk::IntEditSlider::create(context);
            p.heightSlider->setRange(1, 4096);
            p.heightSlider->setLargeStep(100);

            p.typeComboBox = ftk::ComboBox::create(context, ftk::gl::getTextureTypeLabels());

            p.scaleSlider = ftk::FloatEditSlider::create(context);
            p.scaleSlider->setRange(.001F, 100.F);

            auto formLayout = ftk::FormLayout::create(context);
            formLayout->setMarginRole(ftk::SizeRole::Margin);
            formLayout->addRow("Width:", p.widthSlider);
            formLayout->addRow("Height:", p.heightSlider);
            formLayout->addRow("Type:", p.typeComboBox);
            formLayout->addRow("Scale:", p.scaleSlider);
            p.bellows = ftk::Bellows::create(context, getNodeInfo().name, shared_from_this());
            p.bellows->setOpen(true);
            p.bellows->setWidget(formLayout);

            p.widthSlider->setPressedCallback(
                [this](int value, bool pressed)
                {
                    ftk::Size2I size = _node->getAttr("Size");
                    size.w = value;
                    _callback({ { "Size", size } }, pressed);
                });

            p.heightSlider->setPressedCallback(
                [this](int value, bool pressed)
                {
                    ftk::Size2I size = _node->getAttr("Size");
                    size.h = value;
                    _callback({ { "Size", size } }, pressed);
                });

            p.typeComboBox->setIndexCallback(
                [this](int value)
                {
                    _document->command(render::NodeAttrCmd::create(
                        _document->getGraph(),
                        _node,
                        "Type",
                        static_cast<ftk::gl::TextureType>(value)));
                });

            p.scaleSlider->setPressedCallback(
                [this](float value, bool pressed)
                {
                    _callback({ { "Scale", value } }, pressed);
                });

            p.observer = ftk::MapObserver<std::string, nlohmann::json>::create(
                _node->observeAttr(),
                [this](const std::map<std::string, nlohmann::json>& value)
                {
                    FTK_P();
                    auto tmp = value;
                    const ftk::Size2I size = tmp["Size"];
                    p.widthSlider->setValue(size.w);
                    p.heightSlider->setValue(size.h);
                    const ftk::gl::TextureType type = tmp["Type"];
                    p.typeComboBox->setCurrentIndex(static_cast<int>(type));
                    p.scaleSlider->setValue(tmp["Scale"]);
                });
        }

        NoiseNodeWidget::NoiseNodeWidget() :
            _p(new Private)
        {}

        NoiseNodeWidget::~NoiseNodeWidget()
        {}

        std::shared_ptr<NoiseNodeWidget> NoiseNodeWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<NoiseNodeWidget> out(new NoiseNodeWidget);
            out->_init(context, document, node, parent);
            return out;
        }

        render::NodeInfo NoiseNodeWidget::getClassNodeInfo()
        {
            return render::NoiseNode::getClassNodeInfo();
        }

        ftk::Size2I NoiseNodeWidget::getSizeHint() const
        {
            return _p->bellows->getSizeHint();
        }

        void NoiseNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            IInteractionNodeWidget::setGeometry(value);
            _p->bellows->setGeometry(value);
        }

        struct TextNodeWidget::Private
        {
            std::shared_ptr<ftk::TextEdit> textEdit;
            std::shared_ptr<ftk::ComboBox> fontComboBox;
            std::shared_ptr<ftk::IntEditSlider> fontSizeEdit;
            std::shared_ptr<ftk::ColorSwatch> colorSwatch;
            std::shared_ptr<ftk::IntEditSlider> marginEdit;
            std::shared_ptr<ftk::ColorSwatch> backgroundSwatch;
            std::shared_ptr<ftk::Bellows> bellows;

            std::shared_ptr<ftk::MapObserver<std::string, nlohmann::json> > observer;
        };

        void TextNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IInteractionNodeWidget::_init(context, document, node, parent);
            FTK_P();

            p.textEdit = ftk::TextEdit::create(context);

            p.fontComboBox = ftk::ComboBox::create(context, ftk::getFontLabels());

            p.fontSizeEdit = ftk::IntEditSlider::create(context);
            p.fontSizeEdit->setRange(1, 512);

            p.colorSwatch = ftk::ColorSwatch::create(context);
            p.colorSwatch->setEditable(true);

            p.marginEdit = ftk::IntEditSlider::create(context);
            p.marginEdit->setRange(0, 512);

            p.backgroundSwatch = ftk::ColorSwatch::create(context);
            p.backgroundSwatch->setEditable(true);

            auto formLayout = ftk::FormLayout::create(context);
            formLayout->setMarginRole(ftk::SizeRole::Margin);
            formLayout->addRow("Text:", p.textEdit);
            formLayout->addRow("Font:", p.fontComboBox);
            formLayout->addRow("Font size:", p.fontSizeEdit);
            formLayout->addRow("Color:", p.colorSwatch);
            formLayout->addRow("Margin:", p.marginEdit);
            formLayout->addRow("Background:", p.backgroundSwatch);
            p.bellows = ftk::Bellows::create(context, getNodeInfo().name, shared_from_this());
            p.bellows->setOpen(true);
            p.bellows->setWidget(formLayout);

            p.textEdit->setCallback(
                [this](const std::vector<std::string>& text)
                {
                    _document->command(render::NodeAttrCmd::create(
                        _document->getGraph(),
                        _node,
                        "Text",
                        ftk::join(text, '\n')));
                });

            p.fontComboBox->setIndexCallback(
                [this](int value)
                {
                    _document->command(render::NodeAttrCmd::create(
                        _document->getGraph(),
                        _node,
                        "Font",
                        ftk::getFont(static_cast<ftk::Font>(value))));
                });

            p.fontSizeEdit->setPressedCallback(
                [this](int value, bool pressed)
                {
                    _callback({ { "FontSize", value } }, pressed);
                });

            p.colorSwatch->setPressedCallback(
                [this](const ftk::Color4F& value, bool pressed)
                {
                    _callback({ { "Color", value } }, pressed);
                });

            p.marginEdit->setPressedCallback(
                [this](int value, bool pressed)
                {
                    _callback({ { "Margin", value } }, pressed);
                });

            p.backgroundSwatch->setPressedCallback(
                [this](const ftk::Color4F& value, bool pressed)
                {
                    _callback({ { "Background", value } }, pressed);
                });

            p.observer = ftk::MapObserver<std::string, nlohmann::json>::create(
                _node->observeAttr(),
                [this](const std::map<std::string, nlohmann::json>& value)
                {
                    FTK_P();
                    auto tmp = value;
                    p.textEdit->setText(ftk::split(tmp["Text"], '\n', ftk::SplitOptions::KeepEmpty));
                    const std::string fontText = tmp["Font"];
                    int index = 0;
                    for (const auto font : ftk::getFontEnums())
                    {
                        if (fontText == ftk::getFont(font))
                        {
                            break;
                        }
                        ++index;
                    }
                    p.fontComboBox->setCurrentIndex(index);
                    p.fontSizeEdit->setValue(tmp["FontSize"]);
                    p.colorSwatch->setColor(tmp["Color"]);
                    p.marginEdit->setValue(tmp["Margin"]);
                    p.backgroundSwatch->setColor(tmp["Background"]);
                });
        }

        TextNodeWidget::TextNodeWidget() :
            _p(new Private)
        {}

        TextNodeWidget::~TextNodeWidget()
        {}

        std::shared_ptr<TextNodeWidget> TextNodeWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<TextNodeWidget> out(new TextNodeWidget);
            out->_init(context, document, node, parent);
            return out;
        }

        render::NodeInfo TextNodeWidget::getClassNodeInfo()
        {
            return render::TextNode::getClassNodeInfo();
        }

        ftk::Size2I TextNodeWidget::getSizeHint() const
        {
            return _p->bellows->getSizeHint();
        }

        void TextNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            IInteractionNodeWidget::setGeometry(value);
            _p->bellows->setGeometry(value);
        }
    }
}
