// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "GenNodeWidget.h"

#include <ibis/Models/Document.h>

#include <ibis/Render/GenNode.h>
#include <ibis/Render/Graph.h>
#include <ibis/Render/GraphCmd.h>

#include <ftk/UI/ColorSwatch.h>
#include <ftk/UI/Divider.h>
#include <ftk/UI/FormLayout.h>
#include <ftk/UI/IntEdit.h>
#include <ftk/UI/Label.h>
#include <ftk/UI/RowLayout.h>

namespace ibis
{
    namespace ui
    {
        struct SolidColorNodeWidget::Private
        {
            std::shared_ptr<render::NodeAttrCmd> cmd;

            std::shared_ptr<ftk::Label> label;
            std::shared_ptr<ftk::IntEdit> widthEdit;
            std::shared_ptr<ftk::IntEdit> heightEdit;
            std::shared_ptr<ftk::ColorSwatch> colorSwatch;
            std::shared_ptr<ftk::VerticalLayout> layout;

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

            p.label = ftk::Label::create(context, getID());
            p.label->setMarginRole(ftk::SizeRole::MarginSmall);
            p.label->setBackgroundRole(ftk::ColorRole::Button);

            p.widthEdit = ftk::IntEdit::create(context);
            p.widthEdit->setRange(1, 4096);
            p.widthEdit->setStep(10);
            p.widthEdit->setLargeStep(100);

            p.heightEdit = ftk::IntEdit::create(context);
            p.heightEdit->setRange(1, 4096);
            p.heightEdit->setStep(10);
            p.heightEdit->setLargeStep(100);

            p.colorSwatch = ftk::ColorSwatch::create(context);
            p.colorSwatch->setEditable(true);

            p.layout = ftk::VerticalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(ftk::SizeRole::None);
            p.label->setParent(p.layout);
            ftk::Divider::create(context, ftk::Orientation::Vertical, p.layout);
            auto formLayout = ftk::FormLayout::create(context, p.layout);
            formLayout->setMarginRole(ftk::SizeRole::Margin);
            formLayout->addRow("Width:", p.widthEdit);
            formLayout->addRow("Height:", p.heightEdit);
            formLayout->addRow("Color:", p.colorSwatch);

            p.widthEdit->setCallback(
                [this](int value)
                {
                    ftk::Size2I size = _node->getAttr("Size");
                    if (value != size.w)
                    {
                        size.w = value;
                        _document->command(render::NodeAttrCmd::create(
                            _document->getGraph(), _node, "Size", size));
                    }
                });

            p.heightEdit->setCallback(
                [this](int value)
                {
                    ftk::Size2I size = _node->getAttr("Size");
                    if (value != size.h)
                    {
                        size.h = value;
                        _document->command(render::NodeAttrCmd::create(
                            _document->getGraph(), _node, "Size", size));
                    }
                });

            p.colorSwatch->setPressedCallback(
                [this](const ftk::Color4F& value, bool pressed)
                {
                    FTK_P();
                    if (pressed)
                    {
                        if (!p.cmd)
                        {
                            p.cmd = render::NodeAttrCmd::create(
                                _document->getGraph(), _node, "Color", value);
                        }
                        _document->getGraph()->setAttr(_node, "Color", value);
                    }
                    else if (p.cmd)
                    {
                        p.cmd->set(value);
                        _document->command(p.cmd);
                        p.cmd.reset();
                    }
                });

            p.observer = ftk::MapObserver<std::string, nlohmann::json>::create(
                _node->observeAttr(),
                [this](const std::map<std::string, nlohmann::json>& value)
                {
                    FTK_P();
                    ftk::Size2I size;
                    auto i = value.find("Size");
                    if (i != value.end())
                    {
                        size = i->second;
                    }
                    p.widthEdit->setValue(size.w);
                    p.heightEdit->setValue(size.h);

                    ftk::Color4F color;
                    i = value.find("Color");
                    if (i != value.end())
                    {
                        color = i->second;
                    }
                    p.colorSwatch->setColor(color);
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

        std::string SolidColorNodeWidget::getNodeID()
        {
            return render::SolidColorNode::getNodeID();
        }

        ftk::Size2I SolidColorNodeWidget::getSizeHint() const
        {
            return _p->layout->getSizeHint();
        }

        void SolidColorNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            INodeWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }
    }
}
