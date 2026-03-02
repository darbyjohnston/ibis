// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "ColorNodeWidget.h"

#include <ibis/Models/Document.h>

#include <ibis/Render/ColorNode.h>

#include <ftk/UI/ColorSlider.h>
#include <ftk/UI/DoubleEditSlider.h>
#include <ftk/UI/FormLayout.h>
#include <ftk/UI/LevelsSlider.h>
#include <ftk/UI/RowLayout.h>

namespace ibis
{
    namespace ui
    {
        struct BrightnessNodeWidget::Private
        {
            std::shared_ptr<ftk::DoubleEditSlider> valueSlider;
            std::shared_ptr<ftk::FormLayout> layout;

            std::shared_ptr<ftk::MapObserver<std::string, nlohmann::json> > observer;
        };

        void BrightnessNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IInteractionNodeWidget::_init(context, document, node, parent);
            FTK_P();

            p.valueSlider = ftk::DoubleEditSlider::create(context);
            p.valueSlider->setRange(0.0, 4.0);
            p.valueSlider->setDefault(1.0);

            p.layout = ftk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(ftk::SizeRole::Margin);
            p.layout->addRow("Value:", p.valueSlider);

            p.valueSlider->setPressedCallback(
                [this](double value, bool pressed)
                {
                    _callback({ { "Value", value } }, pressed);
                });

            p.observer = ftk::MapObserver<std::string, nlohmann::json>::create(
                _node->observeAttr(),
                [this](const std::map<std::string, nlohmann::json>& value)
                {
                    FTK_P();
                    auto tmp = value;
                    p.valueSlider->setValue(tmp["Value"]);
                });
        }

        BrightnessNodeWidget::BrightnessNodeWidget() :
            _p(new Private)
        {}

        BrightnessNodeWidget::~BrightnessNodeWidget()
        {}

        std::shared_ptr<BrightnessNodeWidget> BrightnessNodeWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<BrightnessNodeWidget> out(new BrightnessNodeWidget);
            out->_init(context, document, node, parent);
            return out;
        }

        render::NodeInfo BrightnessNodeWidget::getClassNodeInfo()
        {
            return render::BrightnessNode::getClassNodeInfo();
        }

        ftk::Size2I BrightnessNodeWidget::getSizeHint() const
        {
            return _p->layout->getSizeHint();
        }

        void BrightnessNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            IInteractionNodeWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        struct ContrastNodeWidget::Private
        {
            std::shared_ptr<ftk::DoubleEditSlider> valueSlider;
            std::shared_ptr<ftk::FormLayout> layout;

            std::shared_ptr<ftk::MapObserver<std::string, nlohmann::json> > observer;
        };

        void ContrastNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IInteractionNodeWidget::_init(context, document, node, parent);
            FTK_P();

            p.valueSlider = ftk::DoubleEditSlider::create(context);
            p.valueSlider->setRange(0.0, 4.0);
            p.valueSlider->setDefault(1.0);

            p.layout = ftk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(ftk::SizeRole::Margin);
            p.layout->addRow("Value:", p.valueSlider);

            p.valueSlider->setPressedCallback(
                [this](double value, bool pressed)
                {
                    _callback({ { "Value", value } }, pressed);
                });

            p.observer = ftk::MapObserver<std::string, nlohmann::json>::create(
                _node->observeAttr(),
                [this](const std::map<std::string, nlohmann::json>& value)
                {
                    FTK_P();
                    auto tmp = value;
                    p.valueSlider->setValue(tmp["Value"]);
                });
        }

        ContrastNodeWidget::ContrastNodeWidget() :
            _p(new Private)
        {}

        ContrastNodeWidget::~ContrastNodeWidget()
        {}

        std::shared_ptr<ContrastNodeWidget> ContrastNodeWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<ContrastNodeWidget> out(new ContrastNodeWidget);
            out->_init(context, document, node, parent);
            return out;
        }

        render::NodeInfo ContrastNodeWidget::getClassNodeInfo()
        {
            return render::ContrastNode::getClassNodeInfo();
        }

        ftk::Size2I ContrastNodeWidget::getSizeHint() const
        {
            return _p->layout->getSizeHint();
        }

        void ContrastNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            IInteractionNodeWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        struct SaturationNodeWidget::Private
        {
            std::shared_ptr<ftk::DoubleEditSlider> valueSlider;
            std::shared_ptr<ftk::FormLayout> layout;

            std::shared_ptr<ftk::MapObserver<std::string, nlohmann::json> > observer;
        };

        void SaturationNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IInteractionNodeWidget::_init(context, document, node, parent);
            FTK_P();

            p.valueSlider = ftk::DoubleEditSlider::create(context);
            p.valueSlider->setRange(0.0, 4.0);
            p.valueSlider->setDefault(1.0);

            p.layout = ftk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(ftk::SizeRole::Margin);
            p.layout->addRow("Value:", p.valueSlider);

            p.valueSlider->setPressedCallback(
                [this](double value, bool pressed)
                {
                    _callback({ { "Value", value } }, pressed);
                });

            p.observer = ftk::MapObserver<std::string, nlohmann::json>::create(
                _node->observeAttr(),
                [this](const std::map<std::string, nlohmann::json>& value)
                {
                    FTK_P();
                    auto tmp = value;
                    p.valueSlider->setValue(tmp["Value"]);
                });
        }

        SaturationNodeWidget::SaturationNodeWidget() :
            _p(new Private)
        {}

        SaturationNodeWidget::~SaturationNodeWidget()
        {}

        std::shared_ptr<SaturationNodeWidget> SaturationNodeWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<SaturationNodeWidget> out(new SaturationNodeWidget);
            out->_init(context, document, node, parent);
            return out;
        }

        render::NodeInfo SaturationNodeWidget::getClassNodeInfo()
        {
            return render::SaturationNode::getClassNodeInfo();
        }

        ftk::Size2I SaturationNodeWidget::getSizeHint() const
        {
            return _p->layout->getSizeHint();
        }

        void SaturationNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            IInteractionNodeWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        struct HueNodeWidget::Private
        {
            std::shared_ptr<ftk::ColorIntEditSlider> valueSlider;
            std::shared_ptr<ftk::FormLayout> layout;

            std::shared_ptr<ftk::MapObserver<std::string, nlohmann::json> > observer;
        };

        void HueNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IInteractionNodeWidget::_init(context, document, node, parent);
            FTK_P();

            p.valueSlider = ftk::ColorIntEditSlider::create(context);
            p.valueSlider->setColors({
                ftk::hue(6.0 / 6.0) * ftk::V4F(1.F, 0.F, 0.F),
                ftk::hue(5.0 / 6.0) * ftk::V4F(1.F, 0.F, 0.F),
                ftk::hue(4.0 / 6.0) * ftk::V4F(1.F, 0.F, 0.F),
                ftk::hue(3.0 / 6.0) * ftk::V4F(1.F, 0.F, 0.F),
                ftk::hue(2.0 / 6.0) * ftk::V4F(1.F, 0.F, 0.F),
                ftk::hue(1.0 / 6.0) * ftk::V4F(1.F, 0.F, 0.F),
                ftk::hue(0.0 / 6.0) * ftk::V4F(1.F, 0.F, 0.F) });
            p.valueSlider->setRange(0, 360);
            p.valueSlider->getModel()->setDefault(0);

            p.layout = ftk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(ftk::SizeRole::Margin);
            p.layout->addRow("Value:", p.valueSlider);

            p.valueSlider->setPressedCallback(
                [this](int value, bool pressed)
                {
                    _callback({ { "Value", value } }, pressed);
                });

            p.observer = ftk::MapObserver<std::string, nlohmann::json>::create(
                _node->observeAttr(),
                [this](const std::map<std::string, nlohmann::json>& value)
                {
                    FTK_P();
                    auto tmp = value;
                    p.valueSlider->setValue(tmp["Value"]);
                });
        }

        HueNodeWidget::HueNodeWidget() :
            _p(new Private)
        {}

        HueNodeWidget::~HueNodeWidget()
        {}

        std::shared_ptr<HueNodeWidget> HueNodeWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<HueNodeWidget> out(new HueNodeWidget);
            out->_init(context, document, node, parent);
            return out;
        }

        render::NodeInfo HueNodeWidget::getClassNodeInfo()
        {
            return render::HueNode::getClassNodeInfo();
        }

        ftk::Size2I HueNodeWidget::getSizeHint() const
        {
            return _p->layout->getSizeHint();
        }

        void HueNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            IInteractionNodeWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        struct LevelsNodeWidget::Private
        {
            std::shared_ptr<ftk::LevelsEditSlider> inSlider;
            std::shared_ptr<ftk::ColorFloatEditSlider> gammaSlider;
            std::shared_ptr<ftk::LevelsEditSlider> outSlider;
            std::shared_ptr<ftk::FormLayout> layout;

            std::shared_ptr<ftk::MapObserver<std::string, nlohmann::json> > observer;
        };

        void LevelsNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IInteractionNodeWidget::_init(context, document, node, parent);
            FTK_P();

            p.inSlider = ftk::LevelsEditSlider::create(context);
            p.inSlider->getModel()->setDefault(ftk::RangeF(0.F, 1.F));

            p.gammaSlider = ftk::ColorFloatEditSlider::create(context);
            p.gammaSlider->setColors({ ftk::V4F(0.F, 0.F, 0.F), ftk::V4F(1.F, 1.F, 1.F) });
            p.gammaSlider->setRange(0.1, 4.0);
            p.gammaSlider->getModel()->setDefault(1.0);

            p.outSlider = ftk::LevelsEditSlider::create(context);
            p.outSlider->getModel()->setDefault(ftk::RangeF(0.F, 1.F));

            p.layout = ftk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(ftk::SizeRole::Margin);
            p.layout->addRow("Input:", p.inSlider);
            p.layout->addRow("Gamma:", p.gammaSlider);
            p.layout->addRow("Output:", p.outSlider);

            p.inSlider->setPressedCallback(
                [this](const ftk::RangeF& value, bool pressed)
                {
                    _callback(
                        {
                            { "InLow", static_cast<double>(value.min()) },
                            { "InHigh", static_cast<double>(value.max()) },
                            { "Gamma", _node->getAttr("Gamma") },
                            { "OutLow", _node->getAttr("OutLow") },
                            { "OutHigh", _node->getAttr("OutHigh") }
                        },
                        pressed);
                });

            p.gammaSlider->setPressedCallback(
                [this](double value, bool pressed)
                {
                    _callback(
                        {
                            { "InLow", _node->getAttr("InLow") },
                            { "InHigh", _node->getAttr("InHigh") },
                            { "Gamma", value },
                            { "OutLow", _node->getAttr("OutLow") },
                            { "OutHigh", _node->getAttr("OutHigh") }
                        },
                        pressed);
                });

            p.outSlider->setPressedCallback(
                [this](const ftk::RangeF& value, bool pressed)
                {
                    _callback(
                        {
                            { "InLow", _node->getAttr("InLow") },
                            { "InHigh", _node->getAttr("InHigh") },
                            { "Gamma", _node->getAttr("Gamma") },
                            { "OutLow", static_cast<double>(value.min()) },
                            { "OutHigh", static_cast<double>(value.max()) }
                        },
                        pressed);
                });

            p.observer = ftk::MapObserver<std::string, nlohmann::json>::create(
                _node->observeAttr(),
                [this](const std::map<std::string, nlohmann::json>& value)
                {
                    FTK_P();
                    auto tmp = value;
                    p.inSlider->setValue(ftk::RangeF(tmp["InLow"], tmp["InHigh"]));
                    p.gammaSlider->setValue(tmp["Gamma"]);
                    p.outSlider->setValue(ftk::RangeF(tmp["OutLow"], tmp["OutHigh"]));
                });
        }

        LevelsNodeWidget::LevelsNodeWidget() :
            _p(new Private)
        {}

        LevelsNodeWidget::~LevelsNodeWidget()
        {}

        std::shared_ptr<LevelsNodeWidget> LevelsNodeWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<LevelsNodeWidget> out(new LevelsNodeWidget);
            out->_init(context, document, node, parent);
            return out;
        }

        render::NodeInfo LevelsNodeWidget::getClassNodeInfo()
        {
            return render::LevelsNode::getClassNodeInfo();
        }

        ftk::Size2I LevelsNodeWidget::getSizeHint() const
        {
            return _p->layout->getSizeHint();
        }

        void LevelsNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            IInteractionNodeWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        struct SoftClipNodeWidget::Private
        {
            std::shared_ptr<ftk::DoubleEditSlider> valueSlider;
            std::shared_ptr<ftk::FormLayout> layout;

            std::shared_ptr<ftk::MapObserver<std::string, nlohmann::json> > observer;
        };

        void SoftClipNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IInteractionNodeWidget::_init(context, document, node, parent);
            FTK_P();

            p.valueSlider = ftk::DoubleEditSlider::create(context);
            p.valueSlider->setRange(0.0, 1.0);
            p.valueSlider->setDefault(0.0);

            p.layout = ftk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(ftk::SizeRole::Margin);
            p.layout->addRow("Value:", p.valueSlider);

            p.valueSlider->setPressedCallback(
                [this](double value, bool pressed)
                {

                    _callback({ { "Value", value } }, pressed);
                });

            p.observer = ftk::MapObserver<std::string, nlohmann::json>::create(
                _node->observeAttr(),
                [this](const std::map<std::string, nlohmann::json>& value)
                {
                    FTK_P();
                    auto tmp = value;
                    p.valueSlider->setValue(tmp["Value"]);
                });
        }

        SoftClipNodeWidget::SoftClipNodeWidget() :
            _p(new Private)
        {}

        SoftClipNodeWidget::~SoftClipNodeWidget()
        {}

        std::shared_ptr<SoftClipNodeWidget> SoftClipNodeWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<SoftClipNodeWidget> out(new SoftClipNodeWidget);
            out->_init(context, document, node, parent);
            return out;
        }

        render::NodeInfo SoftClipNodeWidget::getClassNodeInfo()
        {
            return render::SoftClipNode::getClassNodeInfo();
        }

        ftk::Size2I SoftClipNodeWidget::getSizeHint() const
        {
            return _p->layout->getSizeHint();
        }

        void SoftClipNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            IInteractionNodeWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }
    }
}
