// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "CompNodeWidget.h"

#include <ibis/Models/Document.h>

#include <ibis/Render/CompNode.h>
#include <ibis/Render/Graph.h>
#include <ibis/Render/GraphCmd.h>

#include <ftk/UI/Bellows.h>
#include <ftk/UI/ComboBox.h>
#include <ftk/UI/FormLayout.h>
#include <ftk/UI/IntEditSlider.h>
#include <ftk/UI/RowLayout.h>

namespace ibis
{
    namespace ui
    {
        struct OverNodeWidget::Private
        {
            std::shared_ptr<render::NodeAttrCmd> cmd;

            std::shared_ptr<ftk::ComboBox> modeComboBox;
            std::shared_ptr<ftk::IntEditSlider> xSlider;
            std::shared_ptr<ftk::IntEditSlider> ySlider;
            std::shared_ptr<ftk::Bellows> bellows;

            std::shared_ptr<ftk::MapObserver<std::string, nlohmann::json> > observer;
        };

        void OverNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ibis::models::Document>& document,
            const std::shared_ptr<ibis::render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            INodeWidget::_init(context, document, node, parent);
            FTK_P();

            p.modeComboBox = ftk::ComboBox::create(context, render::getOverModeLabels());

            p.xSlider = ftk::IntEditSlider::create(context);
            p.xSlider->setRange(-8192, 8192);
            p.ySlider = ftk::IntEditSlider::create(context);
            p.ySlider->setRange(-8192, 8192);

            auto formLayout = ftk::FormLayout::create(context);
            formLayout->setMarginRole(ftk::SizeRole::Margin);
            formLayout->addRow("Operator:", p.modeComboBox);
            formLayout->addRow("X offset:", p.xSlider);
            formLayout->addRow("Y offset:", p.ySlider);
            p.bellows = ftk::Bellows::create(context, getNodeInfo().name, shared_from_this());
            p.bellows->setOpen(true);
            p.bellows->setWidget(formLayout);

            p.modeComboBox->setIndexCallback(
                [this](int value)
                {
                    _document->command(render::NodeAttrCmd::create(
                        _document->getGraph(), _node, "Mode", value));
                });

            p.xSlider->setPressedCallback(
                [this](int value, bool pressed)
                {
                    ftk::V2I pos = _node->getAttr("Offset");
                    pos.x = value;
                    _callback({ { "Offset", pos } }, pressed);
                });

            p.ySlider->setPressedCallback(
                [this](int value, bool pressed)
                {
                    ftk::V2I pos = _node->getAttr("Offset");
                    pos.y = value;
                    _callback({ { "Offset", pos } }, pressed);
                });

            p.observer = ftk::MapObserver<std::string, nlohmann::json>::create(
                _node->observeAttr(),
                [this](const std::map<std::string, nlohmann::json>& value)
                {
                    FTK_P();
                    auto tmp = value;
                    p.modeComboBox->setCurrentIndex(static_cast<int>(tmp["Mode"]));
                    const ftk::V2I pos = tmp["Offset"];
                    p.xSlider->setValue(pos.x);
                    p.ySlider->setValue(pos.y);
                });
        }

        OverNodeWidget::OverNodeWidget() :
            _p(new Private)
        {}

        OverNodeWidget::~OverNodeWidget()
        {}

        std::shared_ptr<OverNodeWidget> OverNodeWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ibis::models::Document>& document,
            const std::shared_ptr<ibis::render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<OverNodeWidget> out(new OverNodeWidget);
            out->_init(context, document, node, parent);
            return out;
        }

        render::NodeInfo OverNodeWidget::getClassNodeInfo()
        {
            return render::OverNode::getClassNodeInfo();
        }

        ftk::Size2I OverNodeWidget::getSizeHint() const
        {
            return _p->bellows->getSizeHint();
        }

        void OverNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            INodeWidget::setGeometry(value);
            _p->bellows->setGeometry(value);
        }

        void OverNodeWidget::_callback(
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
