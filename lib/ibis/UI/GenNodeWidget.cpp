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

namespace ibis
{
    namespace ui
    {
        struct SolidColorNodeWidget::Private
        {
            std::shared_ptr<render::NodeAttrCmd> cmd;

            std::shared_ptr<ftk::IntEditSlider> widthSlider;
            std::shared_ptr<ftk::IntEditSlider> heightSlider;
            std::shared_ptr<ftk::ColorSwatch> colorSwatch;
            std::shared_ptr<ftk::Bellows> bellows;

            std::shared_ptr<ftk::MapObserver<std::string, nlohmann::json> > observer;
        };

        void SolidColorNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ibis::models::Document>& document,
            const std::shared_ptr<ibis::render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            INodeWidget::_init(context, document, node, parent);
            FTK_P();

            p.widthSlider = ftk::IntEditSlider::create(context);
            p.widthSlider->setRange(1, 4096);
            p.widthSlider->setStep(10);
            p.widthSlider->setLargeStep(100);

            p.heightSlider = ftk::IntEditSlider::create(context);
            p.heightSlider->setRange(1, 4096);
            p.heightSlider->setStep(10);
            p.heightSlider->setLargeStep(100);

            p.colorSwatch = ftk::ColorSwatch::create(context);
            p.colorSwatch->setEditable(true);

            auto formLayout = ftk::FormLayout::create(context);
            formLayout->setMarginRole(ftk::SizeRole::Margin);
            formLayout->addRow("Width:", p.widthSlider);
            formLayout->addRow("Height:", p.heightSlider);
            formLayout->addRow("Color:", p.colorSwatch);
            p.bellows = ftk::Bellows::create(context, getInfo().name, shared_from_this());
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
            const std::shared_ptr<ibis::models::Document>& document,
            const std::shared_ptr<ibis::render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<SolidColorNodeWidget> out(new SolidColorNodeWidget);
            out->_init(context, document, node, parent);
            return out;
        }

        render::NodeInfo SolidColorNodeWidget::getNodeInfo()
        {
            return render::SolidColorNode::getNodeInfo();
        }

        ftk::Size2I SolidColorNodeWidget::getSizeHint() const
        {
            return _p->bellows->getSizeHint();
        }

        void SolidColorNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            INodeWidget::setGeometry(value);
            _p->bellows->setGeometry(value);
        }

        void SolidColorNodeWidget::_callback(
            const std::vector<std::pair<std::string, nlohmann::json> >& attr,
            bool pressed)
        {
            FTK_P();
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

        struct GradientNodeWidget::Private
        {
            std::shared_ptr<render::NodeAttrCmd> cmd;
            std::shared_ptr<ftk::IntEditSlider> widthSlider;
            std::shared_ptr<ftk::IntEditSlider> heightSlider;
            std::shared_ptr<ftk::ColorSwatch> color0Swatch;
            std::shared_ptr<ftk::ColorSwatch> color1Swatch;
            std::shared_ptr<ftk::ComboBox> orientationComboBox;
            std::shared_ptr<ftk::Bellows> bellows;

            std::shared_ptr<ftk::MapObserver<std::string, nlohmann::json> > observer;
        };

        void GradientNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ibis::models::Document>& document,
            const std::shared_ptr<ibis::render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            INodeWidget::_init(context, document, node, parent);
            FTK_P();

            p.widthSlider = ftk::IntEditSlider::create(context);
            p.widthSlider->setRange(1, 4096);
            p.widthSlider->setStep(10);
            p.widthSlider->setLargeStep(100);

            p.heightSlider = ftk::IntEditSlider::create(context);
            p.heightSlider->setRange(1, 4096);
            p.heightSlider->setStep(10);
            p.heightSlider->setLargeStep(100);

            p.color0Swatch = ftk::ColorSwatch::create(context);
            p.color0Swatch->setEditable(true);

            p.color1Swatch = ftk::ColorSwatch::create(context);
            p.color1Swatch->setEditable(true);

            p.orientationComboBox = ftk::ComboBox::create(context, ftk::getOrientationLabels());

            auto formLayout = ftk::FormLayout::create(context);
            formLayout->setMarginRole(ftk::SizeRole::Margin);
            formLayout->addRow("Width:", p.widthSlider);
            formLayout->addRow("Height:", p.heightSlider);
            formLayout->addRow("Color 0:", p.color0Swatch);
            formLayout->addRow("Color 1:", p.color1Swatch);
            formLayout->addRow("Orientation:", p.orientationComboBox);
            p.bellows = ftk::Bellows::create(context, getInfo().name, shared_from_this());
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
                        _document->getGraph(), _node, "Orientation", static_cast<ftk::Orientation>(value)));
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
            const std::shared_ptr<ibis::models::Document>& document,
            const std::shared_ptr<ibis::render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<GradientNodeWidget> out(new GradientNodeWidget);
            out->_init(context, document, node, parent);
            return out;
        }

        render::NodeInfo GradientNodeWidget::getNodeInfo()
        {
            return render::GradientNode::getNodeInfo();
        }

        ftk::Size2I GradientNodeWidget::getSizeHint() const
        {
            return _p->bellows->getSizeHint();
        }

        void GradientNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            INodeWidget::setGeometry(value);
            _p->bellows->setGeometry(value);
        }

        void GradientNodeWidget::_callback(
            const std::vector<std::pair<std::string, nlohmann::json> >& attr,
            bool pressed)
        {
            FTK_P();
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

        struct NoiseNodeWidget::Private
        {
            std::shared_ptr<render::NodeAttrCmd> cmd;
            std::shared_ptr<ftk::IntEditSlider> widthSlider;
            std::shared_ptr<ftk::IntEditSlider> heightSlider;
            std::shared_ptr<ftk::FloatEditSlider> scaleSlider;
            std::shared_ptr<ftk::Bellows> bellows;

            std::shared_ptr<ftk::MapObserver<std::string, nlohmann::json> > observer;
        };

        void NoiseNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ibis::models::Document>& document,
            const std::shared_ptr<ibis::render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            INodeWidget::_init(context, document, node, parent);
            FTK_P();

            p.widthSlider = ftk::IntEditSlider::create(context);
            p.widthSlider->setRange(1, 4096);
            p.widthSlider->setStep(10);
            p.widthSlider->setLargeStep(100);

            p.heightSlider = ftk::IntEditSlider::create(context);
            p.heightSlider->setRange(1, 4096);
            p.heightSlider->setStep(10);
            p.heightSlider->setLargeStep(100);

            p.scaleSlider = ftk::FloatEditSlider::create(context);
            p.scaleSlider->setRange(.001F, 100.F);

            auto formLayout = ftk::FormLayout::create(context);
            formLayout->setMarginRole(ftk::SizeRole::Margin);
            formLayout->addRow("Width:", p.widthSlider);
            formLayout->addRow("Height:", p.heightSlider);
            formLayout->addRow("Scale:", p.scaleSlider);
            p.bellows = ftk::Bellows::create(context, getInfo().name, shared_from_this());
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
            const std::shared_ptr<ibis::models::Document>& document,
            const std::shared_ptr<ibis::render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<NoiseNodeWidget> out(new NoiseNodeWidget);
            out->_init(context, document, node, parent);
            return out;
        }

        render::NodeInfo NoiseNodeWidget::getNodeInfo()
        {
            return render::NoiseNode::getNodeInfo();
        }

        ftk::Size2I NoiseNodeWidget::getSizeHint() const
        {
            return _p->bellows->getSizeHint();
        }

        void NoiseNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            INodeWidget::setGeometry(value);
            _p->bellows->setGeometry(value);
        }

        void NoiseNodeWidget::_callback(
            const std::vector<std::pair<std::string, nlohmann::json> >& attr,
            bool pressed)
        {
            FTK_P();
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
    }
}
