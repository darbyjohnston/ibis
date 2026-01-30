// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "InputNodeWidget.h"

#include <ibis/Models/Document.h>

#include <ibis/Render/Graph.h>
#include <ibis/Render/GraphCmd.h>
#include <ibis/Render/InputNode.h>

#include <ftk/UI/Bellows.h>
#include <ftk/UI/ComboBox.h>
#include <ftk/UI/FileEdit.h>
#include <ftk/UI/IntEdit.h>
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
            p.bellows = ftk::Bellows::create(context, getInfo().name, shared_from_this());
            p.bellows->setOpen(true);
            p.bellows->setWidget(formLayout);

            p.fileEdit->setCallback(
                [this](const ftk::Path& path)
                {
                    _document->command(render::NodeAttrCmd::create(
                        _document->getGraph(),
                        _node,
                        { { "Path", path.get() } }));
                });

            p.observer = ftk::MapObserver<std::string, nlohmann::json>::create(
                _node->observeAttr(),
                [this](const std::map<std::string, nlohmann::json>& value)
                {
                    FTK_P();
                    auto i = value.find("Path");
                    if (i != value.end())
                    {
                        p.fileEdit->setPath(ftk::Path(i->second));
                    }
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

        render::NodeInfo ImageFileNodeWidget::getNodeInfo()
        {
            return render::ImageFileNode::getNodeInfo();
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

        struct ImageSequenceNodeWidget::Private
        {
            std::shared_ptr<ftk::FileEdit> fileEdit;
            std::shared_ptr<ftk::IntEdit> startFrameEdit;
            std::shared_ptr<ftk::IntEdit> endFrameEdit;
            std::shared_ptr<ftk::ComboBox> loopComboBox;
            std::shared_ptr<ftk::Bellows> bellows;

            bool blockCallbacks = false;
            std::shared_ptr<ftk::MapObserver<std::string, nlohmann::json> > observer;
        };

        void ImageSequenceNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ibis::models::Document>& document,
            const std::shared_ptr<ibis::render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            INodeWidget::_init(context, document, node, parent);
            FTK_P();

            p.fileEdit = ftk::FileEdit::create(context);

            p.startFrameEdit = ftk::IntEdit::create(context);
            p.startFrameEdit->setRange(0, 999999);

            p.endFrameEdit = ftk::IntEdit::create(context);
            p.endFrameEdit->setRange(0, 999999);

            p.loopComboBox = ftk::ComboBox::create(context, render::getInputLoopLabels());

            auto formLayout = ftk::FormLayout::create(context);
            formLayout->setMarginRole(ftk::SizeRole::Margin);
            formLayout->addRow("Path:", p.fileEdit);
            formLayout->addRow("Start frame:", p.startFrameEdit);
            formLayout->addRow("End frame:", p.endFrameEdit);
            formLayout->addRow("Loop:", p.loopComboBox);
            p.bellows = ftk::Bellows::create(context, getInfo().name, shared_from_this());
            p.bellows->setOpen(true);
            p.bellows->setWidget(formLayout);

            p.fileEdit->setCallback(
                [this](const ftk::Path& path)
                {
                    ftk::Path tmp = path;
                    if (tmp.hasNum() && !tmp.isSeq())
                    {
                        tmp = ftk::expandSeq(tmp);
                    }
                    int startFrame = 0;
                    int endFrame = 0;
                    if (tmp.getFrames().has_value())
                    {
                        startFrame = tmp.getFrames()->min();
                        endFrame = tmp.getFrames()->max();
                    }
                    _document->command(render::NodeAttrCmd::create(
                        _document->getGraph(),
                        _node,
                        {
                            { "Path", path.get() },
                            { "StartFrame", startFrame },
                            { "EndFrame", endFrame }
                        }));
                });

            p.startFrameEdit->setCallback(
                [this](int value)
                {
                    _document->command(render::NodeAttrCmd::create(
                        _document->getGraph(),
                        _node,
                        { { "StartFrame", value } }));
                });

            p.endFrameEdit->setCallback(
                [this](int value)
                {
                    _document->command(render::NodeAttrCmd::create(
                        _document->getGraph(),
                        _node,
                        { { "EndFrame", value } }));
                });

            p.loopComboBox->setIndexCallback(
                [this](int value)
                {
                    _document->command(render::NodeAttrCmd::create(
                        _document->getGraph(),
                        _node,
                        { { "Loop", static_cast<render::InputLoop>(value) } }));
                });

            p.observer = ftk::MapObserver<std::string, nlohmann::json>::create(
                _node->observeAttr(),
                [this](const std::map<std::string, nlohmann::json>& value)
                {
                    FTK_P();
                    auto i = value.find("Path");
                    if (i != value.end())
                    {
                        p.fileEdit->setPath(ftk::Path(i->second));
                    }
                    i = value.find("StartFrame");
                    if (i != value.end())
                    {
                        p.startFrameEdit->setValue(i->second);
                    }
                    i = value.find("EndFrame");
                    if (i != value.end())
                    {
                        p.endFrameEdit->setValue(i->second);
                    }
                    i = value.find("Loop");
                    if (i != value.end())
                    {
                        p.loopComboBox->setCurrentIndex(static_cast<int>(i->second));
                    }
                });
        }

        ImageSequenceNodeWidget::ImageSequenceNodeWidget() :
            _p(new Private)
        {}

        ImageSequenceNodeWidget::~ImageSequenceNodeWidget()
        {}

        std::shared_ptr<ImageSequenceNodeWidget> ImageSequenceNodeWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ibis::models::Document>& document,
            const std::shared_ptr<ibis::render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<ImageSequenceNodeWidget> out(new ImageSequenceNodeWidget);
            out->_init(context, document, node, parent);
            return out;
        }

        render::NodeInfo ImageSequenceNodeWidget::getNodeInfo()
        {
            return render::ImageSequenceNode::getNodeInfo();
        }

        ftk::Size2I ImageSequenceNodeWidget::getSizeHint() const
        {
            return _p->bellows->getSizeHint();
        }

        void ImageSequenceNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            INodeWidget::setGeometry(value);
            _p->bellows->setGeometry(value);
        }
    }
}
