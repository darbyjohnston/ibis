// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "CompNodeWidget.h"

#include <ibis/Models/Document.h>

#include <ibis/Render/CompNode.h>
#include <ibis/Render/Graph.h>
#include <ibis/Render/GraphCmd.h>

#include <ftk/UI/ComboBox.h>
#include <ftk/UI/FormLayout.h>
#include <ftk/UI/IntEditShuttle.h>
#include <ftk/UI/RowLayout.h>

namespace ibis
{
    namespace ui
    {
        struct OverNodeWidget::Private
        {
            std::shared_ptr<ftk::ComboBox> modeComboBox;
            std::shared_ptr<ftk::IntEditShuttle> xEdit;
            std::shared_ptr<ftk::IntEditShuttle> yEdit;
            std::shared_ptr<ftk::FormLayout> layout;

            std::shared_ptr<ftk::MapObserver<std::string, nlohmann::json> > observer;
        };

        void OverNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IInteractionNodeWidget::_init(context, document, node, parent);
            FTK_P();

            p.modeComboBox = ftk::ComboBox::create(context, render::getOverModeLabels());

            p.xEdit = ftk::IntEditShuttle::create(context);
            p.xEdit->setRange(-4096, 4096);
            p.xEdit->setDefault(0);

            p.yEdit = ftk::IntEditShuttle::create(context);
            p.yEdit->setRange(-4096, 4096);
            p.yEdit->setDefault(0);

            p.layout = ftk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(ftk::SizeRole::Margin);
            p.layout->addRow("Operator:", p.modeComboBox);
            p.layout->addRow("X offset:", p.xEdit);
            p.layout->addRow("Y offset:", p.yEdit);

            p.modeComboBox->setIndexCallback(
                [this](int value)
                {
                    _document->command(render::NodeAttrCmd::create(
                        _document->getGraph(), _node, "Mode", value));
                });

            p.xEdit->setPressedCallback(
                [this](int value, bool pressed)
                {
                    ftk::V2I pos = _node->getAttr("Offset");
                    pos.x = value;
                    _callback({ { "Offset", pos } }, pressed);
                });

            p.yEdit->setPressedCallback(
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
                    p.xEdit->setValue(pos.x);
                    p.yEdit->setValue(pos.y);
                });
        }

        OverNodeWidget::OverNodeWidget() :
            _p(new Private)
        {}

        OverNodeWidget::~OverNodeWidget()
        {}

        std::shared_ptr<OverNodeWidget> OverNodeWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
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
            return _p->layout->getSizeHint();
        }

        void OverNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            IInteractionNodeWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }
    }
}
