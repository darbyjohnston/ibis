// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "InputNodeWidget.h"

#include <ibis/Models/Document.h>

#include <ibis/Render/Graph.h>
#include <ibis/Render/GraphCmd.h>
#include <ibis/Render/InputNode.h>

#include <ftk/UI/Divider.h>
#include <ftk/UI/FileEdit.h>
#include <ftk/UI/FormLayout.h>
#include <ftk/UI/Label.h>
#include <ftk/UI/RowLayout.h>

namespace ibis
{
    namespace ui
    {
        struct ImageFileNodeWidget::Private
        {
            std::shared_ptr<ftk::Label> label;
            std::shared_ptr<ftk::FileEdit> fileEdit;
            std::shared_ptr<ftk::VerticalLayout> layout;

            std::shared_ptr<ftk::MapObserver<std::string, nlohmann::json> > observer;
        };

        void ImageFileNodeWidget::_init(
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

            p.fileEdit = ftk::FileEdit::create(context);

            p.layout = ftk::VerticalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(ftk::SizeRole::None);
            p.label->setParent(p.layout);
            ftk::Divider::create(context, ftk::Orientation::Vertical, p.layout);
            auto formLayout = ftk::FormLayout::create(context, p.layout);
            formLayout->setMarginRole(ftk::SizeRole::Margin);
            formLayout->addRow("Path:", p.fileEdit);

            p.fileEdit->setCallback(
                [this](const ftk::Path& path)
                {
                    _document->command(render::NodeAttrCmd::create(
                        _document->getGraph(), _node, "Path", path.get()));
                });

            p.observer = ftk::MapObserver<std::string, nlohmann::json>::create(
                _node->observeAttr(),
                [this](const std::map<std::string, nlohmann::json>& value)
                {
                    FTK_P();
                    std::string path;
                    auto i = value.find("Path");
                    if (i != value.end())
                    {
                        path = i->second;
                    }
                    p.fileEdit->setPath(ftk::Path(path));
                });
        }

        ImageFileNodeWidget::ImageFileNodeWidget() :
            _p(new Private)
        {}

        ImageFileNodeWidget::~ImageFileNodeWidget()
        {}

        std::shared_ptr<ImageFileNodeWidget> ImageFileNodeWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ibis::models::Document>& document,
            const std::shared_ptr<ibis::render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<ImageFileNodeWidget> out(new ImageFileNodeWidget);
            out->_init(context, document, node, parent);
            return out;
        }

        std::string ImageFileNodeWidget::getNodeID()
        {
            return render::ImageFileNode::getNodeID();
        }

        ftk::Size2I ImageFileNodeWidget::getSizeHint() const
        {
            return _p->layout->getSizeHint();
        }

        void ImageFileNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            INodeWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }
    }
}
