// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "ColorNodeWidget.h"

#include <ibis/Models/Document.h>

#include <ibis/Render/ColorNode.h>

#include <ftk/UI/Bellows.h>
#include <ftk/UI/DoubleEditSlider.h>
#include <ftk/UI/FormLayout.h>
#include <ftk/UI/RowLayout.h>

namespace ibis
{
    namespace ui
    {
        struct BrightnessNodeWidget::Private
        {
            std::shared_ptr<ftk::DoubleEditSlider> valueSlider;
            std::shared_ptr<ftk::Bellows> bellows;

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
            p.valueSlider->setDefaultValue(1.0);

            auto formLayout = ftk::FormLayout::create(context);
            formLayout->setMarginRole(ftk::SizeRole::Margin);
            formLayout->addRow("Value:", p.valueSlider);
            p.bellows = ftk::Bellows::create(context, getNodeInfo().name, shared_from_this());
            p.bellows->setOpen(true);
            p.bellows->setWidget(formLayout);

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
            return _p->bellows->getSizeHint();
        }

        void BrightnessNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            IInteractionNodeWidget::setGeometry(value);
            _p->bellows->setGeometry(value);
        }

        struct ContrastNodeWidget::Private
        {
            std::shared_ptr<ftk::DoubleEditSlider> valueSlider;
            std::shared_ptr<ftk::Bellows> bellows;

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
            p.valueSlider->setDefaultValue(1.0);

            auto formLayout = ftk::FormLayout::create(context);
            formLayout->setMarginRole(ftk::SizeRole::Margin);
            formLayout->addRow("Value:", p.valueSlider);
            p.bellows = ftk::Bellows::create(context, getNodeInfo().name, shared_from_this());
            p.bellows->setOpen(true);
            p.bellows->setWidget(formLayout);

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
            return _p->bellows->getSizeHint();
        }

        void ContrastNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            IInteractionNodeWidget::setGeometry(value);
            _p->bellows->setGeometry(value);
        }

        struct SaturationNodeWidget::Private
        {
            std::shared_ptr<ftk::DoubleEditSlider> valueSlider;
            std::shared_ptr<ftk::Bellows> bellows;

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
            p.valueSlider->setDefaultValue(1.0);

            auto formLayout = ftk::FormLayout::create(context);
            formLayout->setMarginRole(ftk::SizeRole::Margin);
            formLayout->addRow("Value:", p.valueSlider);
            p.bellows = ftk::Bellows::create(context, getNodeInfo().name, shared_from_this());
            p.bellows->setOpen(true);
            p.bellows->setWidget(formLayout);

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
            return _p->bellows->getSizeHint();
        }

        void SaturationNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            IInteractionNodeWidget::setGeometry(value);
            _p->bellows->setGeometry(value);
        }

        struct HueNodeWidget::Private
        {
            std::shared_ptr<ftk::DoubleEditSlider> valueSlider;
            std::shared_ptr<ftk::Bellows> bellows;

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

            p.valueSlider = ftk::DoubleEditSlider::create(context);
            p.valueSlider->setRange(0.0, 360.0);
            p.valueSlider->setDefaultValue(0.0);

            auto formLayout = ftk::FormLayout::create(context);
            formLayout->setMarginRole(ftk::SizeRole::Margin);
            formLayout->addRow("Value:", p.valueSlider);
            p.bellows = ftk::Bellows::create(context, getNodeInfo().name, shared_from_this());
            p.bellows->setOpen(true);
            p.bellows->setWidget(formLayout);

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
            return _p->bellows->getSizeHint();
        }

        void HueNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            IInteractionNodeWidget::setGeometry(value);
            _p->bellows->setGeometry(value);
        }

        struct LevelsNodeWidget::Private
        {
            std::map<std::string, std::shared_ptr<ftk::DoubleEditSlider> > sliders;
            std::shared_ptr<ftk::Bellows> bellows;

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

            p.sliders["InLow"] = ftk::DoubleEditSlider::create(context);
            p.sliders["InLow"]->setRange(0.0, 1.0);
            p.sliders["InLow"]->setDefaultValue(0.0);
            p.sliders["InHigh"] = ftk::DoubleEditSlider::create(context);
            p.sliders["InHigh"]->setRange(0.0, 1.0);
            p.sliders["InHigh"]->setDefaultValue(1.0);
            p.sliders["Gamma"] = ftk::DoubleEditSlider::create(context);
            p.sliders["Gamma"]->setRange(0.1, 4.0);
            p.sliders["Gamma"]->setDefaultValue(1.0);
            p.sliders["OutLow"] = ftk::DoubleEditSlider::create(context);
            p.sliders["OutLow"]->setRange(0.0, 1.0);
            p.sliders["OutLow"]->setDefaultValue(0.0);
            p.sliders["OutHigh"] = ftk::DoubleEditSlider::create(context);
            p.sliders["OutHigh"]->setRange(0.0, 1.0);
            p.sliders["OutHigh"]->setDefaultValue(1.0);

            auto formLayout = ftk::FormLayout::create(context);
            formLayout->setMarginRole(ftk::SizeRole::Margin);
            formLayout->addRow("In low:", p.sliders["InLow"]);
            formLayout->addRow("In high:", p.sliders["InHigh"]);
            formLayout->addRow("Gamma:", p.sliders["Gamma"]);
            formLayout->addRow("Out low:", p.sliders["OutLow"]);
            formLayout->addRow("Out high:", p.sliders["OutHigh"]);
            p.bellows = ftk::Bellows::create(context, getNodeInfo().name, shared_from_this());
            p.bellows->setOpen(true);
            p.bellows->setWidget(formLayout);

            p.sliders["InLow"]->setPressedCallback(
                [this](double value, bool pressed)
                {
                    _callback(
                        {
                            { "InLow", value },
                            { "InHigh", _node->getAttr("InHigh") },
                            { "Gamma", _node->getAttr("Gamma") },
                            { "OutLow", _node->getAttr("OutLow") },
                            { "OutHigh", _node->getAttr("OutHigh") }
                        },
                        pressed);
                });
            p.sliders["InHigh"]->setPressedCallback(
                [this](double value, bool pressed)
                {
                    _callback(
                        {
                            { "InLow", _node->getAttr("InLow") },
                            { "InHigh", value },
                            { "Gamma", _node->getAttr("Gamma") },
                            { "OutLow", _node->getAttr("OutLow") },
                            { "OutHigh", _node->getAttr("OutHigh") }
                        },
                        pressed);
                });
            p.sliders["Gamma"]->setPressedCallback(
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
            p.sliders["OutLow"]->setPressedCallback(
                [this](double value, bool pressed)
                {
                    _callback(
                        {
                            { "InLow", _node->getAttr("InLow") },
                            { "InHigh", _node->getAttr("InHigh") },
                            { "Gamma", _node->getAttr("Gamma") },
                            { "OutLow", value },
                            { "OutHigh", _node->getAttr("OutHigh") }
                        },
                        pressed);
                });
            p.sliders["OutHigh"]->setPressedCallback(
                [this](double value, bool pressed)
                {
                    _callback(
                        {
                            { "InLow", _node->getAttr("InLow") },
                            { "InHigh", _node->getAttr("InHigh") },
                            { "Gamma", _node->getAttr("Gamma") },
                            { "OutLow", _node->getAttr("OutLow") },
                            { "OutHigh", value }
                        },
                        pressed);
                });

            p.observer = ftk::MapObserver<std::string, nlohmann::json>::create(
                _node->observeAttr(),
                [this](const std::map<std::string, nlohmann::json>& value)
                {
                    FTK_P();
                    auto tmp = value;
                    p.sliders["InLow"]->setValue(tmp["InLow"]);
                    p.sliders["InHigh"]->setValue(tmp["InHigh"]);
                    p.sliders["Gamma"]->setValue(tmp["Gamma"]);
                    p.sliders["OutLow"]->setValue(tmp["OutLow"]);
                    p.sliders["OutHigh"]->setValue(tmp["OutHigh"]);
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
            return _p->bellows->getSizeHint();
        }

        void LevelsNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            IInteractionNodeWidget::setGeometry(value);
            _p->bellows->setGeometry(value);
        }

        struct SoftClipNodeWidget::Private
        {
            std::shared_ptr<ftk::DoubleEditSlider> valueSlider;
            std::shared_ptr<ftk::Bellows> bellows;

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
            p.valueSlider->setDefaultValue(0.0);

            auto formLayout = ftk::FormLayout::create(context);
            formLayout->setMarginRole(ftk::SizeRole::Margin);
            formLayout->addRow("Value:", p.valueSlider);
            p.bellows = ftk::Bellows::create(context, getNodeInfo().name, shared_from_this());
            p.bellows->setOpen(true);
            p.bellows->setWidget(formLayout);

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
            return _p->bellows->getSizeHint();
        }

        void SoftClipNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            IInteractionNodeWidget::setGeometry(value);
            _p->bellows->setGeometry(value);
        }
    }
}
