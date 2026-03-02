// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "TransformNodeWidget.h"

#include <ibis/Models/Document.h>

#include <ibis/Render/Graph.h>
#include <ibis/Render/GraphCmd.h>
#include <ibis/Render/TransformNode.h>

#include <ftk/UI/CheckBox.h>
#include <ftk/UI/FloatEditSlider.h>
#include <ftk/UI/IntEditShuttle.h>
#include <ftk/UI/FormLayout.h>
#include <ftk/UI/RowLayout.h>

namespace ibis
{
    namespace ui
    {
        struct ResizeNodeWidget::Private
        {
            std::shared_ptr<ftk::IntEditShuttle> widthEdit;
            std::shared_ptr<ftk::IntEditShuttle> heightEdit;
            std::shared_ptr<ftk::FormLayout> layout;

            std::shared_ptr<ftk::MapObserver<std::string, nlohmann::json> > observer;
        };

        void ResizeNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IInteractionNodeWidget::_init(context, document, node, parent);
            FTK_P();

            p.widthEdit = ftk::IntEditShuttle::create(context);
            p.widthEdit->setRange(1, 8192);

            p.heightEdit = ftk::IntEditShuttle::create(context);
            p.heightEdit->setRange(1, 8192);

            p.layout = ftk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(ftk::SizeRole::Margin);
            p.layout->addRow("Width:", p.widthEdit);
            p.layout->addRow("Height:", p.heightEdit);

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
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
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
            return _p->layout->getSizeHint();
        }

        void ResizeNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            IInteractionNodeWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        struct CropNodeWidget::Private
        {
            std::shared_ptr<ftk::IntEditShuttle> xEdit;
            std::shared_ptr<ftk::IntEditShuttle> yEdit;
            std::shared_ptr<ftk::IntEditShuttle> widthEdit;
            std::shared_ptr<ftk::IntEditShuttle> heightEdit;
            std::shared_ptr<ftk::FormLayout> layout;

            std::shared_ptr<ftk::MapObserver<std::string, nlohmann::json> > observer;
        };

        void CropNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IInteractionNodeWidget::_init(context, document, node, parent);
            FTK_P();

            p.xEdit = ftk::IntEditShuttle::create(context);
            p.xEdit->setRange(0, 8192);

            p.yEdit = ftk::IntEditShuttle::create(context);
            p.yEdit->setRange(0, 8192);

            p.widthEdit = ftk::IntEditShuttle::create(context);
            p.widthEdit->setRange(1, 8192);

            p.heightEdit = ftk::IntEditShuttle::create(context);
            p.heightEdit->setRange(1, 8192);

            p.layout = ftk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(ftk::SizeRole::Margin);
            p.layout->addRow("X:", p.xEdit);
            p.layout->addRow("Y:", p.yEdit);
            p.layout->addRow("Width:", p.widthEdit);
            p.layout->addRow("Height:", p.heightEdit);

            p.xEdit->setPressedCallback(
                [this](int value, bool pressed)
                {
                    _callback({ { "X", value } }, pressed);
                });

            p.yEdit->setPressedCallback(
                [this](int value, bool pressed)
                {
                    _callback({ { "Y", value } }, pressed);
                });

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
                    p.xEdit->setValue(tmp["X"]);
                    p.yEdit->setValue(tmp["Y"]);
                    p.widthEdit->setValue(tmp["Width"]);
                    p.heightEdit->setValue(tmp["Height"]);
                });
        }

        CropNodeWidget::CropNodeWidget() :
            _p(new Private)
        {}

        CropNodeWidget::~CropNodeWidget()
        {}

        std::shared_ptr<CropNodeWidget> CropNodeWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<CropNodeWidget> out(new CropNodeWidget);
            out->_init(context, document, node, parent);
            return out;
        }

        render::NodeInfo CropNodeWidget::getClassNodeInfo()
        {
            return render::CropNode::getClassNodeInfo();
        }

        ftk::Size2I CropNodeWidget::getSizeHint() const
        {
            return _p->layout->getSizeHint();
        }

        void CropNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            IInteractionNodeWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        struct MirrorNodeWidget::Private
        {
            std::shared_ptr<ftk::CheckBox> hCheckBox;
            std::shared_ptr<ftk::CheckBox> vCheckBox;
            std::shared_ptr<ftk::FormLayout> layout;

            std::shared_ptr<ftk::MapObserver<std::string, nlohmann::json> > observer;
        };

        void MirrorNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            INodeWidget::_init(context, document, node, parent);
            FTK_P();

            p.hCheckBox = ftk::CheckBox::create(context);
            p.hCheckBox->setHStretch(ftk::Stretch::Expanding);

            p.vCheckBox = ftk::CheckBox::create(context);
            p.vCheckBox->setHStretch(ftk::Stretch::Expanding);

            p.layout = ftk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(ftk::SizeRole::Margin);
            p.layout->addRow("Horizontal:", p.hCheckBox);
            p.layout->addRow("Vertical:", p.vCheckBox);

            p.hCheckBox->setCheckedCallback(
                [this](bool value)
                {
                    _document->command(render::NodeAttrCmd::create(
                        _document->getGraph(),
                        _node,
                        "Horizontal",
                        value));
                });

            p.vCheckBox->setCheckedCallback(
                [this](bool value)
                {
                    _document->command(render::NodeAttrCmd::create(
                        _document->getGraph(),
                        _node,
                        "Vertical",
                        value));
                });

            p.observer = ftk::MapObserver<std::string, nlohmann::json>::create(
                _node->observeAttr(),
                [this](const std::map<std::string, nlohmann::json>& value)
                {
                    FTK_P();
                    auto tmp = value;
                    p.hCheckBox->setChecked(tmp["Horizontal"]);
                    p.vCheckBox->setChecked(tmp["Vertical"]);
                });
        }

        MirrorNodeWidget::MirrorNodeWidget() :
            _p(new Private)
        {}

        MirrorNodeWidget::~MirrorNodeWidget()
        {}

        std::shared_ptr<MirrorNodeWidget> MirrorNodeWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<MirrorNodeWidget> out(new MirrorNodeWidget);
            out->_init(context, document, node, parent);
            return out;
        }

        render::NodeInfo MirrorNodeWidget::getClassNodeInfo()
        {
            return render::MirrorNode::getClassNodeInfo();
        }

        ftk::Size2I MirrorNodeWidget::getSizeHint() const
        {
            return _p->layout->getSizeHint();
        }

        void MirrorNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            INodeWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        struct RotateNodeWidget::Private
        {
            std::shared_ptr<ftk::FloatEditSlider> rotateEdit;
            std::shared_ptr<ftk::FormLayout> layout;

            std::shared_ptr<ftk::MapObserver<std::string, nlohmann::json> > observer;
        };

        void RotateNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IInteractionNodeWidget::_init(context, document, node, parent);
            FTK_P();

            p.rotateEdit = ftk::FloatEditSlider::create(context);
            p.rotateEdit->setRange(0.F, 360.F);

            p.layout = ftk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(ftk::SizeRole::Margin);
            p.layout->addRow("Rotate:", p.rotateEdit);

            p.rotateEdit->setPressedCallback(
                [this](float value, bool pressed)
                {
                    _callback({ { "Rotate", value } }, pressed);
                });

            p.observer = ftk::MapObserver<std::string, nlohmann::json>::create(
                _node->observeAttr(),
                [this](const std::map<std::string, nlohmann::json>& value)
                {
                    FTK_P();
                    auto tmp = value;
                    p.rotateEdit->setValue(tmp["Rotate"]);
                });
        }

        RotateNodeWidget::RotateNodeWidget() :
            _p(new Private)
        {}

        RotateNodeWidget::~RotateNodeWidget()
        {}

        std::shared_ptr<RotateNodeWidget> RotateNodeWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<RotateNodeWidget> out(new RotateNodeWidget);
            out->_init(context, document, node, parent);
            return out;
        }

        render::NodeInfo RotateNodeWidget::getClassNodeInfo()
        {
            return render::RotateNode::getClassNodeInfo();
        }

        ftk::Size2I RotateNodeWidget::getSizeHint() const
        {
            return _p->layout->getSizeHint();
        }

        void RotateNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            IInteractionNodeWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }
    }
}
