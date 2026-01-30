// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "MathNodeWidget.h"

#include <ibis/Models/Document.h>

#include <ibis/Render/Graph.h>
#include <ibis/Render/GraphCmd.h>
#include <ibis/Render/MathNode.h>

#include <ftk/UI/Bellows.h>
#include <ftk/UI/ComboBox.h>
#include <ftk/UI/DoubleEditSlider.h>
#include <ftk/UI/FormLayout.h>
#include <ftk/UI/RowLayout.h>

namespace ibis
{
    namespace ui
    {
        struct ArithmeticNodeWidget::Private
        {
            std::shared_ptr<render::NodeAttrCmd> cmd;

            std::shared_ptr<ftk::ComboBox> operatorComboBox;
            std::shared_ptr<ftk::DoubleEditSlider> valueSlider;
            std::shared_ptr<ftk::Bellows> bellows;

            std::shared_ptr<ftk::MapObserver<std::string, nlohmann::json> > observer;
        };

        void ArithmeticNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ibis::models::Document>& document,
            const std::shared_ptr<ibis::render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            INodeWidget::_init(context, document, node, parent);
            FTK_P();

            p.operatorComboBox = ftk::ComboBox::create(context, render::getArithmeticOperatorLabels());

            p.valueSlider = ftk::DoubleEditSlider::create(context);
            p.valueSlider->setRange(0.0, 2.0);

            auto formLayout = ftk::FormLayout::create(context);
            formLayout->setMarginRole(ftk::SizeRole::Margin);
            formLayout->addRow("Operator:", p.operatorComboBox);
            formLayout->addRow("Value:", p.valueSlider);
            p.bellows = ftk::Bellows::create(context, getInfo().name, shared_from_this());
            p.bellows->setOpen(true);
            p.bellows->setWidget(formLayout);

            p.operatorComboBox->setIndexCallback(
                [this](int value)
                {
                    _document->command(render::NodeAttrCmd::create(
                        _document->getGraph(), _node, "Operator", value));
                });

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
                    auto tmp = value;
                    p.operatorComboBox->setCurrentIndex(static_cast<int>(tmp["Operator"]));
                    p.valueSlider->setValue(tmp["Value"]);
                });
        }

        ArithmeticNodeWidget::ArithmeticNodeWidget() :
            _p(new Private)
        {}

        ArithmeticNodeWidget::~ArithmeticNodeWidget()
        {}

        std::shared_ptr<ArithmeticNodeWidget> ArithmeticNodeWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ibis::models::Document>& document,
            const std::shared_ptr<ibis::render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<ArithmeticNodeWidget> out(new ArithmeticNodeWidget);
            out->_init(context, document, node, parent);
            return out;
        }

        render::NodeInfo ArithmeticNodeWidget::getNodeInfo()
        {
            return render::ArithmeticNode::getNodeInfo();
        }

        ftk::Size2I ArithmeticNodeWidget::getSizeHint() const
        {
            return _p->bellows->getSizeHint();
        }

        void ArithmeticNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            INodeWidget::setGeometry(value);
            _p->bellows->setGeometry(value);
        }
    }
}
