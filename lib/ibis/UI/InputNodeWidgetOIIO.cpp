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
#include <ftk/UI/Icon.h>
#include <ftk/UI/IntEdit.h>
#include <ftk/UI/FormLayout.h>
#include <ftk/UI/RowLayout.h>

#include <ftk/Core/Format.h>
#include <ftk/Core/String.h>

namespace ibis
{
    namespace ui
    {
        std::string getExtTooltip(const std::vector<std::string>& exts)
        {
            std::vector<std::string> lines;
            std::vector<std::string> tmp;
            for (const auto& ext : exts)
            {
                if (tmp.size() >= 10)
                {
                    lines.push_back(ftk::join(tmp, ", "));
                    tmp.clear();
                }
                else
                {
                    tmp.push_back(ext);
                }
            }
            if (!tmp.empty())
            {
                lines.push_back(ftk::join(tmp, ", "));
            }
            return ftk::join(lines, ",\n");
        }

        struct ImageInputNodeWidget::Private
        {
            std::shared_ptr<ftk::FileEdit> fileEdit;
            std::shared_ptr<ftk::ComboBox> subImageComboBox;
            std::shared_ptr<ftk::ComboBox> channelsComboBox;
            std::shared_ptr<ftk::Bellows> bellows;

            std::shared_ptr<ftk::ListObserver<std::string> > subImagesObserver;
            std::shared_ptr<ftk::ListObserver<std::string> > channelsObserver;
            std::shared_ptr<ftk::MapObserver<std::string, nlohmann::json> > attrObserver;
        };

        void ImageInputNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            INodeWidget::_init(context, document, node, parent);
            FTK_P();

            p.fileEdit = ftk::FileEdit::create(context);

            auto infoIcon = ftk::Icon::create(context, "Info");
            infoIcon->setTooltip(
                ftk::Format("Supported file extensions:\n{0}").
                arg(getExtTooltip(render::ImageInputNode::getExts())));

            p.subImageComboBox = ftk::ComboBox::create(context);

            p.channelsComboBox = ftk::ComboBox::create(context);

            auto formLayout = ftk::FormLayout::create(context);
            formLayout->setMarginRole(ftk::SizeRole::Margin);
            auto hLayout = ftk::HorizontalLayout::create(context);
            hLayout->setSpacingRole(ftk::SizeRole::SpacingTool);
            hLayout->setHStretch(ftk::Stretch::Expanding);
            p.fileEdit->setParent(hLayout);
            infoIcon->setParent(hLayout);
            formLayout->addRow("Path:", hLayout);
            formLayout->addRow("Sub-image:", p.subImageComboBox);
            formLayout->addRow("Channels:", p.channelsComboBox);
            p.bellows = ftk::Bellows::create(context, getNodeInfo().name, shared_from_this());
            p.bellows->setOpen(true);
            p.bellows->setWidget(formLayout);

            p.fileEdit->setCallback(
                [this](const ftk::Path& path)
                {
                    // Check that the path has actually changed, the callback
                    // can also be triggered by focus changes.
                    if (path.get() != _node->getAttr("Path"))
                    {
                        _document->command(render::NodeAttrCmd::create(
                            _document->getGraph(),
                            _node,
                            "Path",
                            path.get()));
                    }
                });

            p.subImageComboBox->setIndexCallback(
                [this](int value)
                {
                    _document->command(render::NodeAttrCmd::create(
                        _document->getGraph(),
                        _node,
                        "SubImage",
                        value));
                });

            p.channelsComboBox->setIndexCallback(
                [this](int value)
                {
                    _document->command(render::NodeAttrCmd::create(
                        _document->getGraph(),
                        _node,
                        "ChannelGroup",
                        value));
                });

            auto ImageInputNode = std::dynamic_pointer_cast<render::ImageInputNode>(node);
            p.subImagesObserver = ftk::ListObserver<std::string>::create(
                ImageInputNode->observeSubImages(),
                [this](const std::vector<std::string>& value)
                {
                    _p->subImageComboBox->setItems(value);
                });

            p.channelsObserver = ftk::ListObserver<std::string>::create(
                ImageInputNode->observeChannels(),
                [this](const std::vector<std::string>& value)
                {
                    _p->channelsComboBox->setItems(value);
                });

            p.attrObserver = ftk::MapObserver<std::string, nlohmann::json>::create(
                node->observeAttr(),
                [this](const std::map<std::string, nlohmann::json>& value)
                {
                    FTK_P();
                    auto tmp = value;
                    p.fileEdit->setPath(ftk::Path(tmp["Path"]));
                    p.subImageComboBox->setCurrentIndex(tmp["SubImage"]);
                    p.channelsComboBox->setCurrentIndex(tmp["ChannelGroup"]);
                });
        }

        ImageInputNodeWidget::ImageInputNodeWidget() :
            _p(new Private)
        {}

        ImageInputNodeWidget::~ImageInputNodeWidget()
        {}

        std::shared_ptr<ImageInputNodeWidget> ImageInputNodeWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<ImageInputNodeWidget> out(new ImageInputNodeWidget);
            out->_init(context, document, node, parent);
            return out;
        }

        render::NodeInfo ImageInputNodeWidget::getClassNodeInfo()
        {
            return render::ImageInputNode::getClassNodeInfo();
        }

        ftk::Size2I ImageInputNodeWidget::getSizeHint() const
        {
            return _p->bellows->getSizeHint();
        }

        void ImageInputNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            INodeWidget::setGeometry(value);
            _p->bellows->setGeometry(value);
        }

        struct SequenceInputNodeWidget::Private
        {
            std::shared_ptr<ftk::FileEdit> fileEdit;
            std::shared_ptr<ftk::ComboBox> subImageComboBox;
            std::shared_ptr<ftk::ComboBox> channelsComboBox;
            std::shared_ptr<ftk::IntEdit> startFrameEdit;
            std::shared_ptr<ftk::IntEdit> endFrameEdit;
            std::shared_ptr<ftk::ComboBox> loopComboBox;
            std::shared_ptr<ftk::Bellows> bellows;

            std::shared_ptr<ftk::ListObserver<std::string> > subImagesObserver;
            std::shared_ptr<ftk::ListObserver<std::string> > channelsObserver;
            std::shared_ptr<ftk::MapObserver<std::string, nlohmann::json> > attrObserver;
        };

        void SequenceInputNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            INodeWidget::_init(context, document, node, parent);
            FTK_P();

            p.fileEdit = ftk::FileEdit::create(context);

            auto infoIcon = ftk::Icon::create(context, "Info");
            infoIcon->setTooltip(
                ftk::Format("Supported file extensions:\n{0}").
                arg(getExtTooltip(render::SequenceInputNode::getExts())));

            p.subImageComboBox = ftk::ComboBox::create(context);

            p.channelsComboBox = ftk::ComboBox::create(context);

            p.startFrameEdit = ftk::IntEdit::create(context);
            p.startFrameEdit->setRange(0, 999999);

            p.endFrameEdit = ftk::IntEdit::create(context);
            p.endFrameEdit->setRange(0, 999999);

            p.loopComboBox = ftk::ComboBox::create(context, render::getInputLoopLabels());

            auto formLayout = ftk::FormLayout::create(context);
            formLayout->setMarginRole(ftk::SizeRole::Margin);
            auto hLayout = ftk::HorizontalLayout::create(context);
            hLayout->setSpacingRole(ftk::SizeRole::SpacingTool);
            hLayout->setHStretch(ftk::Stretch::Expanding);
            p.fileEdit->setParent(hLayout);
            infoIcon->setParent(hLayout);
            formLayout->addRow("Path:", hLayout);
            formLayout->addRow("Sub-image:", p.subImageComboBox);
            formLayout->addRow("Channels:", p.channelsComboBox);
            formLayout->addRow("Start frame:", p.startFrameEdit);
            formLayout->addRow("End frame:", p.endFrameEdit);
            formLayout->addRow("Loop:", p.loopComboBox);
            p.bellows = ftk::Bellows::create(context, getNodeInfo().name, shared_from_this());
            p.bellows->setOpen(true);
            p.bellows->setWidget(formLayout);

            p.fileEdit->setCallback(
                [this](const ftk::Path& path)
                {
                    // Check that the path has actually changed, the callback
                    // can also be triggered by focus changes.
                    if (path.get() != _node->getAttr("Path"))
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
                    }
                });

            p.subImageComboBox->setIndexCallback(
                [this](int value)
                {
                    _document->command(render::NodeAttrCmd::create(
                        _document->getGraph(),
                        _node,
                        "SubImage",
                        value));
                });

            p.channelsComboBox->setIndexCallback(
                [this](int value)
                {
                    _document->command(render::NodeAttrCmd::create(
                        _document->getGraph(),
                        _node,
                        "ChannelGroup",
                        value));
                });

            p.startFrameEdit->setCallback(
                [this](int value)
                {
                    _document->command(render::NodeAttrCmd::create(
                        _document->getGraph(),
                        _node,
                        "StartFrame",
                        value));
                });

            p.endFrameEdit->setCallback(
                [this](int value)
                {
                    _document->command(render::NodeAttrCmd::create(
                        _document->getGraph(),
                        _node,
                        "EndFrame",
                        value));
                });

            p.loopComboBox->setIndexCallback(
                [this](int value)
                {
                    _document->command(render::NodeAttrCmd::create(
                        _document->getGraph(),
                        _node,
                        "Loop",
                        static_cast<render::InputLoop>(value)));
                });

            auto sequenceInputNode = std::dynamic_pointer_cast<render::SequenceInputNode>(node);
            p.subImagesObserver = ftk::ListObserver<std::string>::create(
                sequenceInputNode->observeSubImages(),
                [this](const std::vector<std::string>& value)
                {
                    _p->subImageComboBox->setItems(value);
                });

            p.channelsObserver = ftk::ListObserver<std::string>::create(
                sequenceInputNode->observeChannels(),
                [this](const std::vector<std::string>& value)
                {
                    _p->channelsComboBox->setItems(value);
                });

            p.attrObserver = ftk::MapObserver<std::string, nlohmann::json>::create(
                node->observeAttr(),
                [this](const std::map<std::string, nlohmann::json>& value)
                {
                    FTK_P();
                    auto tmp = value;
                    p.fileEdit->setPath(ftk::Path(tmp["Path"]));
                    p.subImageComboBox->setCurrentIndex(tmp["SubImage"]);
                    p.channelsComboBox->setCurrentIndex(tmp["ChannelGroup"]);
                    p.startFrameEdit->setValue(tmp["StartFrame"]);
                    p.endFrameEdit->setValue(tmp["EndFrame"]);
                    p.loopComboBox->setCurrentIndex(static_cast<int>(tmp["Loop"]));
                });
        }

        SequenceInputNodeWidget::SequenceInputNodeWidget() :
            _p(new Private)
        {}

        SequenceInputNodeWidget::~SequenceInputNodeWidget()
        {}

        std::shared_ptr<SequenceInputNodeWidget> SequenceInputNodeWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<SequenceInputNodeWidget> out(new SequenceInputNodeWidget);
            out->_init(context, document, node, parent);
            return out;
        }

        render::NodeInfo SequenceInputNodeWidget::getClassNodeInfo()
        {
            return render::SequenceInputNode::getClassNodeInfo();
        }

        ftk::Size2I SequenceInputNodeWidget::getSizeHint() const
        {
            return _p->bellows->getSizeHint();
        }

        void SequenceInputNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            INodeWidget::setGeometry(value);
            _p->bellows->setGeometry(value);
        }
    }
}
