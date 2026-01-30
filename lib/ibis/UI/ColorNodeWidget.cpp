// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "ColorNodeWidget.h"

#include <ibis/Models/Document.h>

#include <ibis/Render/ColorNode.h>
#include <ibis/Render/Graph.h>
#include <ibis/Render/GraphCmd.h>

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
            std::shared_ptr<render::NodeAttrCmd> cmd;

            std::shared_ptr<ftk::DoubleEditSlider> valueSlider;
            std::shared_ptr<ftk::Bellows> bellows;

            std::shared_ptr<ftk::MapObserver<std::string, nlohmann::json> > observer;
        };

        void BrightnessNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ibis::models::Document>& document,
            const std::shared_ptr<ibis::render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            INodeWidget::_init(context, document, node, parent);
            FTK_P();

            p.valueSlider = ftk::DoubleEditSlider::create(context);
            p.valueSlider->setRange(0.0, 4.0);
            p.valueSlider->setDefaultValue(1.0);

            auto formLayout = ftk::FormLayout::create(context);
            formLayout->setMarginRole(ftk::SizeRole::Margin);
            formLayout->addRow("Value:", p.valueSlider);
            p.bellows = ftk::Bellows::create(context, getInfo().name, shared_from_this());
            p.bellows->setOpen(true);
            p.bellows->setWidget(formLayout);

            p.valueSlider->setPressedCallback(
                [this](double value, bool pressed)
                {
                    FTK_P();
                    if (pressed)
                    {
                        if (!p.cmd)
                        {
                            p.cmd = render::NodeAttrCmd::create(
                                _document->getGraph(), _node, "Value", value);
                        }
                        _document->getGraph()->setAttr(_node, "Value", value);
                    }
                    else if (p.cmd)
                    {
                        p.cmd->set("Value", value);
                        _document->command(p.cmd);
                        p.cmd.reset();
                    }
                    else
                    {
                        _document->command(render::NodeAttrCmd::create(
                            _document->getGraph(), _node, "Value", value));
                    }
                });

            p.observer = ftk::MapObserver<std::string, nlohmann::json>::create(
                _node->observeAttr(),
                [this](const std::map<std::string, nlohmann::json>& value)
                {
                    FTK_P();
                    const auto i = value.find("Value");
                    if (i != value.end())
                    {
                        p.valueSlider->setValue(i->second);
                    }
                });
        }

        BrightnessNodeWidget::BrightnessNodeWidget() :
            _p(new Private)
        {}

        BrightnessNodeWidget::~BrightnessNodeWidget()
        {}

        std::shared_ptr<BrightnessNodeWidget> BrightnessNodeWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ibis::models::Document>& document,
            const std::shared_ptr<ibis::render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<BrightnessNodeWidget> out(new BrightnessNodeWidget);
            out->_init(context, document, node, parent);
            return out;
        }

        render::NodeInfo BrightnessNodeWidget::getNodeInfo()
        {
            return render::BrightnessNode::getNodeInfo();
        }

        ftk::Size2I BrightnessNodeWidget::getSizeHint() const
        {
            return _p->bellows->getSizeHint();
        }

        void BrightnessNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            INodeWidget::setGeometry(value);
            _p->bellows->setGeometry(value);
        }

        struct ContrastNodeWidget::Private
        {
            std::shared_ptr<render::NodeAttrCmd> cmd;

            std::shared_ptr<ftk::DoubleEditSlider> valueSlider;
            std::shared_ptr<ftk::Bellows> bellows;

            std::shared_ptr<ftk::MapObserver<std::string, nlohmann::json> > observer;
        };

        void ContrastNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ibis::models::Document>& document,
            const std::shared_ptr<ibis::render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            INodeWidget::_init(context, document, node, parent);
            FTK_P();

            p.valueSlider = ftk::DoubleEditSlider::create(context);
            p.valueSlider->setRange(0.0, 4.0);
            p.valueSlider->setDefaultValue(1.0);

            auto formLayout = ftk::FormLayout::create(context);
            formLayout->setMarginRole(ftk::SizeRole::Margin);
            formLayout->addRow("Value:", p.valueSlider);
            p.bellows = ftk::Bellows::create(context, getInfo().name, shared_from_this());
            p.bellows->setOpen(true);
            p.bellows->setWidget(formLayout);

            p.valueSlider->setPressedCallback(
                [this](double value, bool pressed)
                {
                    FTK_P();
                    if (pressed)
                    {
                        if (!p.cmd)
                        {
                            p.cmd = render::NodeAttrCmd::create(
                                _document->getGraph(), _node, "Value", value);
                        }
                        _document->getGraph()->setAttr(_node, "Value", value);
                    }
                    else if (p.cmd)
                    {
                        p.cmd->set("Value", value);
                        _document->command(p.cmd);
                        p.cmd.reset();
                    }
                    else
                    {
                        _document->command(render::NodeAttrCmd::create(
                            _document->getGraph(), _node, "Value", value));
                    }
                });

            p.observer = ftk::MapObserver<std::string, nlohmann::json>::create(
                _node->observeAttr(),
                [this](const std::map<std::string, nlohmann::json>& value)
                {
                    FTK_P();
                    const auto i = value.find("Value");
                    if (i != value.end())
                    {
                        p.valueSlider->setValue(i->second);
                    }
                });
        }

        ContrastNodeWidget::ContrastNodeWidget() :
            _p(new Private)
        {}

        ContrastNodeWidget::~ContrastNodeWidget()
        {}

        std::shared_ptr<ContrastNodeWidget> ContrastNodeWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ibis::models::Document>& document,
            const std::shared_ptr<ibis::render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<ContrastNodeWidget> out(new ContrastNodeWidget);
            out->_init(context, document, node, parent);
            return out;
        }

        render::NodeInfo ContrastNodeWidget::getNodeInfo()
        {
            return render::ContrastNode::getNodeInfo();
        }

        ftk::Size2I ContrastNodeWidget::getSizeHint() const
        {
            return _p->bellows->getSizeHint();
        }

        void ContrastNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            INodeWidget::setGeometry(value);
            _p->bellows->setGeometry(value);
        }

        struct SaturationNodeWidget::Private
        {
            std::shared_ptr<render::NodeAttrCmd> cmd;

            std::shared_ptr<ftk::DoubleEditSlider> valueSlider;
            std::shared_ptr<ftk::Bellows> bellows;

            std::shared_ptr<ftk::MapObserver<std::string, nlohmann::json> > observer;
        };

        void SaturationNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ibis::models::Document>& document,
            const std::shared_ptr<ibis::render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            INodeWidget::_init(context, document, node, parent);
            FTK_P();

            p.valueSlider = ftk::DoubleEditSlider::create(context);
            p.valueSlider->setRange(0.0, 4.0);
            p.valueSlider->setDefaultValue(1.0);

            auto formLayout = ftk::FormLayout::create(context);
            formLayout->setMarginRole(ftk::SizeRole::Margin);
            formLayout->addRow("Value:", p.valueSlider);
            p.bellows = ftk::Bellows::create(context, getInfo().name, shared_from_this());
            p.bellows->setOpen(true);
            p.bellows->setWidget(formLayout);

            p.valueSlider->setPressedCallback(
                [this](double value, bool pressed)
                {
                    FTK_P();
                    if (pressed)
                    {
                        if (!p.cmd)
                        {
                            p.cmd = render::NodeAttrCmd::create(
                                _document->getGraph(), _node, "Value", value);
                        }
                        _document->getGraph()->setAttr(_node, "Value", value);
                    }
                    else if (p.cmd)
                    {
                        p.cmd->set("Value", value);
                        _document->command(p.cmd);
                        p.cmd.reset();
                    }
                    else
                    {
                        _document->command(render::NodeAttrCmd::create(
                            _document->getGraph(), _node, "Value", value));
                    }
                });

            p.observer = ftk::MapObserver<std::string, nlohmann::json>::create(
                _node->observeAttr(),
                [this](const std::map<std::string, nlohmann::json>& value)
                {
                    FTK_P();
                    const auto i = value.find("Value");
                    if (i != value.end())
                    {
                        p.valueSlider->setValue(i->second);
                    }
                });
        }

        SaturationNodeWidget::SaturationNodeWidget() :
            _p(new Private)
        {}

        SaturationNodeWidget::~SaturationNodeWidget()
        {}

        std::shared_ptr<SaturationNodeWidget> SaturationNodeWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ibis::models::Document>& document,
            const std::shared_ptr<ibis::render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<SaturationNodeWidget> out(new SaturationNodeWidget);
            out->_init(context, document, node, parent);
            return out;
        }

        render::NodeInfo SaturationNodeWidget::getNodeInfo()
        {
            return render::SaturationNode::getNodeInfo();
        }

        ftk::Size2I SaturationNodeWidget::getSizeHint() const
        {
            return _p->bellows->getSizeHint();
        }

        void SaturationNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            INodeWidget::setGeometry(value);
            _p->bellows->setGeometry(value);
        }

        struct TintNodeWidget::Private
        {
            std::shared_ptr<render::NodeAttrCmd> cmd;

            std::shared_ptr<ftk::DoubleEditSlider> valueSlider;
            std::shared_ptr<ftk::Bellows> bellows;

            std::shared_ptr<ftk::MapObserver<std::string, nlohmann::json> > observer;
        };

        void TintNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ibis::models::Document>& document,
            const std::shared_ptr<ibis::render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            INodeWidget::_init(context, document, node, parent);
            FTK_P();

            p.valueSlider = ftk::DoubleEditSlider::create(context);
            p.valueSlider->setRange(0.0, 360.0);
            p.valueSlider->setDefaultValue(0.0);

            auto formLayout = ftk::FormLayout::create(context);
            formLayout->setMarginRole(ftk::SizeRole::Margin);
            formLayout->addRow("Value:", p.valueSlider);
            p.bellows = ftk::Bellows::create(context, getInfo().name, shared_from_this());
            p.bellows->setOpen(true);
            p.bellows->setWidget(formLayout);

            p.valueSlider->setPressedCallback(
                [this](double value, bool pressed)
                {
                    FTK_P();
                    if (pressed)
                    {
                        if (!p.cmd)
                        {
                            p.cmd = render::NodeAttrCmd::create(
                                _document->getGraph(), _node, "Value", value);
                        }
                        _document->getGraph()->setAttr(_node, "Value", value);
                    }
                    else if (p.cmd)
                    {
                        p.cmd->set("Value", value);
                        _document->command(p.cmd);
                        p.cmd.reset();
                    }
                    else
                    {
                        _document->command(render::NodeAttrCmd::create(
                            _document->getGraph(), _node, "Value", value));
                    }
                });

            p.observer = ftk::MapObserver<std::string, nlohmann::json>::create(
                _node->observeAttr(),
                [this](const std::map<std::string, nlohmann::json>& value)
                {
                    FTK_P();
                    const auto i = value.find("Value");
                    if (i != value.end())
                    {
                        p.valueSlider->setValue(i->second);
                    }
                });
        }

        TintNodeWidget::TintNodeWidget() :
            _p(new Private)
        {}

        TintNodeWidget::~TintNodeWidget()
        {}

        std::shared_ptr<TintNodeWidget> TintNodeWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ibis::models::Document>& document,
            const std::shared_ptr<ibis::render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<TintNodeWidget> out(new TintNodeWidget);
            out->_init(context, document, node, parent);
            return out;
        }

        render::NodeInfo TintNodeWidget::getNodeInfo()
        {
            return render::TintNode::getNodeInfo();
        }

        ftk::Size2I TintNodeWidget::getSizeHint() const
        {
            return _p->bellows->getSizeHint();
        }

        void TintNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            INodeWidget::setGeometry(value);
            _p->bellows->setGeometry(value);
        }

        struct LevelsNodeWidget::Private
        {
            std::shared_ptr<render::NodeAttrCmd> cmd;

            std::map<std::string, std::shared_ptr<ftk::DoubleEditSlider> > sliders;
            std::shared_ptr<ftk::Bellows> bellows;

            std::shared_ptr<ftk::MapObserver<std::string, nlohmann::json> > observer;
        };

        void LevelsNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ibis::models::Document>& document,
            const std::shared_ptr<ibis::render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            INodeWidget::_init(context, document, node, parent);
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
            p.bellows = ftk::Bellows::create(context, getInfo().name, shared_from_this());
            p.bellows->setOpen(true);
            p.bellows->setWidget(formLayout);

            p.sliders["InLow"]->setPressedCallback(
                [this](double value, bool pressed)
                {
                    _callback(
                        value,
                        _node->getAttr("InHigh"),
                        _node->getAttr("Gamma"),
                        _node->getAttr("OutLow"),
                        _node->getAttr("OutHigh"),
                        pressed);
                });
            p.sliders["InHigh"]->setPressedCallback(
                [this](double value, bool pressed)
                {
                    _callback(
                        _node->getAttr("InLow"),
                        value,
                        _node->getAttr("Gamma"),
                        _node->getAttr("OutLow"),
                        _node->getAttr("OutHigh"),
                        pressed);
                });
            p.sliders["Gamma"]->setPressedCallback(
                [this](double value, bool pressed)
                {
                    _callback(
                        _node->getAttr("InLow"),
                        _node->getAttr("InHigh"),
                        value,
                        _node->getAttr("OutLow"),
                        _node->getAttr("OutHigh"),
                        pressed);
                });
            p.sliders["OutLow"]->setPressedCallback(
                [this](double value, bool pressed)
                {
                    _callback(
                        _node->getAttr("InLow"),
                        _node->getAttr("InHigh"),
                        _node->getAttr("Gamma"),
                        value,
                        _node->getAttr("OutHigh"),
                        pressed);
                });
            p.sliders["OutHigh"]->setPressedCallback(
                [this](double value, bool pressed)
                {
                    _callback(
                        _node->getAttr("InLow"),
                        _node->getAttr("InHigh"),
                        _node->getAttr("Gamma"),
                        _node->getAttr("OutLow"),
                        value,
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
            const std::shared_ptr<ibis::models::Document>& document,
            const std::shared_ptr<ibis::render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<LevelsNodeWidget> out(new LevelsNodeWidget);
            out->_init(context, document, node, parent);
            return out;
        }

        render::NodeInfo LevelsNodeWidget::getNodeInfo()
        {
            return render::LevelsNode::getNodeInfo();
        }

        ftk::Size2I LevelsNodeWidget::getSizeHint() const
        {
            return _p->bellows->getSizeHint();
        }

        void LevelsNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            INodeWidget::setGeometry(value);
            _p->bellows->setGeometry(value);
        }

        void LevelsNodeWidget::_callback(
            float inLow,
            float inHigh,
            float gamma,
            float outLow,
            float outHigh,
            bool pressed)
        {
            FTK_P();
            const std::vector<std::pair<std::string, nlohmann::json> > attr =
            {
                { "InLow", inLow },
                { "InHigh", inHigh },
                { "Gamma", gamma },
                { "OutLow", outLow },
                { "OutHigh", outHigh }
            };
            if (pressed)
            {
                if (!p.cmd)
                {
                    p.cmd = render::NodeAttrCmd::create(
                        _document->getGraph(), _node, attr);
                }
                _document->getGraph()->setAttr(_node, attr);
            }
            else if (p.cmd)
            {
                p.cmd->set(attr);
                _document->command(p.cmd);
                p.cmd.reset();
            }
            else
            {
                _document->command(render::NodeAttrCmd::create(
                    _document->getGraph(), _node, attr));
            }
        }

        struct SoftClipNodeWidget::Private
        {
            std::shared_ptr<render::NodeAttrCmd> cmd;

            std::shared_ptr<ftk::DoubleEditSlider> valueSlider;
            std::shared_ptr<ftk::Bellows> bellows;

            std::shared_ptr<ftk::MapObserver<std::string, nlohmann::json> > observer;
        };

        void SoftClipNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ibis::models::Document>& document,
            const std::shared_ptr<ibis::render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            INodeWidget::_init(context, document, node, parent);
            FTK_P();

            p.valueSlider = ftk::DoubleEditSlider::create(context);
            p.valueSlider->setRange(0.0, 1.0);
            p.valueSlider->setDefaultValue(0.0);

            auto formLayout = ftk::FormLayout::create(context);
            formLayout->setMarginRole(ftk::SizeRole::Margin);
            formLayout->addRow("Value:", p.valueSlider);
            p.bellows = ftk::Bellows::create(context, getInfo().name, shared_from_this());
            p.bellows->setOpen(true);
            p.bellows->setWidget(formLayout);

            p.valueSlider->setPressedCallback(
                [this](double value, bool pressed)
                {
                    FTK_P();
                    if (pressed)
                    {
                        if (!p.cmd)
                        {
                            p.cmd = render::NodeAttrCmd::create(
                                _document->getGraph(), _node, "Value", value);
                        }
                        _document->getGraph()->setAttr(_node, "Value", value);
                    }
                    else if (p.cmd)
                    {
                        p.cmd->set("Value", value);
                        _document->command(p.cmd);
                        p.cmd.reset();
                    }
                    else
                    {
                        _document->command(render::NodeAttrCmd::create(
                            _document->getGraph(), _node, "Value", value));
                    }
                });

            p.observer = ftk::MapObserver<std::string, nlohmann::json>::create(
                _node->observeAttr(),
                [this](const std::map<std::string, nlohmann::json>& value)
                {
                    FTK_P();
                    const auto i = value.find("Value");
                    if (i != value.end())
                    {
                        p.valueSlider->setValue(i->second);
                    }
                });
        }

        SoftClipNodeWidget::SoftClipNodeWidget() :
            _p(new Private)
        {}

        SoftClipNodeWidget::~SoftClipNodeWidget()
        {}

        std::shared_ptr<SoftClipNodeWidget> SoftClipNodeWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ibis::models::Document>& document,
            const std::shared_ptr<ibis::render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<SoftClipNodeWidget> out(new SoftClipNodeWidget);
            out->_init(context, document, node, parent);
            return out;
        }

        render::NodeInfo SoftClipNodeWidget::getNodeInfo()
        {
            return render::SoftClipNode::getNodeInfo();
        }

        ftk::Size2I SoftClipNodeWidget::getSizeHint() const
        {
            return _p->bellows->getSizeHint();
        }

        void SoftClipNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            INodeWidget::setGeometry(value);
            _p->bellows->setGeometry(value);
        }
    }
}
