// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "TransformNodeWidget.h"

#include <ibis/Models/Document.h>

#include <ibis/Render/Graph.h>
#include <ibis/Render/GraphCmd.h>
#include <ibis/Render/TransformNode.h>

#include <ftk/UI/Bellows.h>
#include <ftk/UI/IntEditSlider.h>
#include <ftk/UI/FormLayout.h>
#include <ftk/UI/RowLayout.h>

namespace ibis
{
    namespace ui
    {
        struct ResizeNodeWidget::Private
        {
            std::shared_ptr<render::NodeAttrCmd> cmd;

            std::shared_ptr<ftk::IntEditSlider> widthEdit;
            std::shared_ptr<ftk::IntEditSlider> heightEdit;
            std::shared_ptr<ftk::Bellows> bellows;

            std::shared_ptr<ftk::MapObserver<std::string, nlohmann::json> > observer;
        };

        void ResizeNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ibis::models::Document>& document,
            const std::shared_ptr<ibis::render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IInteractionNodeWidget::_init(context, document, node, parent);
            FTK_P();

            p.widthEdit = ftk::IntEditSlider::create(context);
            p.widthEdit->setRange(1, 8192);

            p.heightEdit = ftk::IntEditSlider::create(context);
            p.heightEdit->setRange(1, 8192);

            auto formLayout = ftk::FormLayout::create(context);
            formLayout->setMarginRole(ftk::SizeRole::Margin);
            formLayout->addRow("Width:", p.widthEdit);
            formLayout->addRow("Height:", p.heightEdit);
            p.bellows = ftk::Bellows::create(context, getNodeInfo().name, shared_from_this());
            p.bellows->setOpen(true);
            p.bellows->setWidget(formLayout);

            p.widthEdit->setPressedCallback(
                [this](int value, bool pressed)
                {
                    _callback({ { "Width", value } }, pressed);
                });

            p.heightEdit->setPressedCallback(
                [this](int value, bool pressed)
                {
                    _callback({ { "Height", value } }, pressed);
                });

            p.observer = ftk::MapObserver<std::string, nlohmann::json>::create(
                _node->observeAttr(),
                [this](const std::map<std::string, nlohmann::json>& value)
                {
                    FTK_P();
                    auto tmp = value;
                    p.widthEdit->setValue(tmp["Width"]);
                    p.heightEdit->setValue(tmp["Height"]);
                });
        }

        ResizeNodeWidget::ResizeNodeWidget() :
            _p(new Private)
        {}

        ResizeNodeWidget::~ResizeNodeWidget()
        {}

        std::shared_ptr<ResizeNodeWidget> ResizeNodeWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ibis::models::Document>& document,
            const std::shared_ptr<ibis::render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<ResizeNodeWidget> out(new ResizeNodeWidget);
            out->_init(context, document, node, parent);
            return out;
        }

        render::NodeInfo ResizeNodeWidget::getClassNodeInfo()
        {
            return render::ResizeNode::getClassNodeInfo();
        }

        ftk::Size2I ResizeNodeWidget::getSizeHint() const
        {
            return _p->bellows->getSizeHint();
        }

        void ResizeNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            IInteractionNodeWidget::setGeometry(value);
            _p->bellows->setGeometry(value);
        }
    }
}
