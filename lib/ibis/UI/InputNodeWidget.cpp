// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "InputNodeWidget.h"

#include <ibis/Models/Document.h>

#include <ibis/Render/Graph.h>
#include <ibis/Render/GraphCmd.h>
#include <ibis/Render/InputNode.h>

#include <ftk/UI/Bellows.h>
#include <ftk/UI/FileEdit.h>
#include <ftk/UI/FormLayout.h>
#include <ftk/UI/RowLayout.h>

namespace ibis
{
    namespace ui
    {
        struct ImageFileNodeWidget::Private
        {
            std::shared_ptr<ftk::FileEdit> fileEdit;
            std::shared_ptr<ftk::Bellows> bellows;

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

            p.fileEdit = ftk::FileEdit::create(context);

            auto formLayout = ftk::FormLayout::create(context);
            formLayout->setMarginRole(ftk::SizeRole::Margin);
            formLayout->addRow("Path:", p.fileEdit);
            p.bellows = ftk::Bellows::create(context, getID(), shared_from_this());
            p.bellows->setOpen(true);
            p.bellows->setWidget(formLayout);

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
            return _p->bellows->getSizeHint();
        }

        void ImageFileNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            INodeWidget::setGeometry(value);
            _p->bellows->setGeometry(value);
        }

        struct ImageFileSequenceNodeWidget::Private
        {
            std::shared_ptr<ftk::FileEdit> fileEdit;
            std::shared_ptr<ftk::Bellows> bellows;

            std::shared_ptr<ftk::MapObserver<std::string, nlohmann::json> > observer;
        };

        void ImageFileSequenceNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ibis::models::Document>& document,
            const std::shared_ptr<ibis::render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            INodeWidget::_init(context, document, node, parent);
            FTK_P();

            p.fileEdit = ftk::FileEdit::create(context);

            auto formLayout = ftk::FormLayout::create(context);
            formLayout->setMarginRole(ftk::SizeRole::Margin);
            formLayout->addRow("Path:", p.fileEdit);
            p.bellows = ftk::Bellows::create(context, getID(), shared_from_this());
            p.bellows->setOpen(true);
            p.bellows->setWidget(formLayout);

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

        ImageFileSequenceNodeWidget::ImageFileSequenceNodeWidget() :
            _p(new Private)
        {}

        ImageFileSequenceNodeWidget::~ImageFileSequenceNodeWidget()
        {}

        std::shared_ptr<ImageFileSequenceNodeWidget> ImageFileSequenceNodeWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ibis::models::Document>& document,
            const std::shared_ptr<ibis::render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<ImageFileSequenceNodeWidget> out(new ImageFileSequenceNodeWidget);
            out->_init(context, document, node, parent);
            return out;
        }

        std::string ImageFileSequenceNodeWidget::getNodeID()
        {
            return render::ImageFileSequenceNode::getNodeID();
        }

        ftk::Size2I ImageFileSequenceNodeWidget::getSizeHint() const
        {
            return _p->bellows->getSizeHint();
        }

        void ImageFileSequenceNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            INodeWidget::setGeometry(value);
            _p->bellows->setGeometry(value);
        }
    }
}
