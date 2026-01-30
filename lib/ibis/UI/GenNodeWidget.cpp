// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "GenNodeWidget.h"

#include <ibis/Models/Document.h>

#include <ibis/Render/GenNode.h>
#include <ibis/Render/Graph.h>
#include <ibis/Render/GraphCmd.h>

#include <ftk/UI/Bellows.h>
#include <ftk/UI/ColorSwatch.h>
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
    }
}
