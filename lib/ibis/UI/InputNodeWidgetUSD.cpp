// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "InputNodeWidget.h"

#include <ibis/Models/Document.h>

#include <ibis/Render/Graph.h>
#include <ibis/Render/GraphCmd.h>
#include <ibis/Render/InputNode.h>

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
        struct USDInputNodeWidget::Private
        {
            std::shared_ptr<ftk::FileEdit> fileEdit;
            std::shared_ptr<ftk::IntEdit> startFrameEdit;
            std::shared_ptr<ftk::IntEdit> endFrameEdit;
            std::shared_ptr<ftk::ComboBox> loopComboBox;
            std::shared_ptr<ftk::FormLayout> layout;

            std::shared_ptr<ftk::MapObserver<std::string, nlohmann::json> > attrObserver;
        };

        void USDInputNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
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

            p.layout = ftk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(ftk::SizeRole::Margin);
            p.layout->addRow("Path:", p.fileEdit);
            p.layout->addRow("Start frame:", p.startFrameEdit);
            p.layout->addRow("End frame:", p.endFrameEdit);
            p.layout->addRow("Loop:", p.loopComboBox);

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

            p.attrObserver = ftk::MapObserver<std::string, nlohmann::json>::create(
                node->observeAttr(),
                [this](const std::map<std::string, nlohmann::json>& value)
                {
                    FTK_P();
                    auto tmp = value;
                    p.fileEdit->setPath(ftk::Path(tmp["Path"]));
                    p.startFrameEdit->setValue(tmp["StartFrame"]);
                    p.endFrameEdit->setValue(tmp["EndFrame"]);
                    p.loopComboBox->setCurrentIndex(static_cast<int>(tmp["Loop"]));
                });
        }

        USDInputNodeWidget::USDInputNodeWidget() :
            _p(new Private)
        {}

        USDInputNodeWidget::~USDInputNodeWidget()
        {}

        std::shared_ptr<USDInputNodeWidget> USDInputNodeWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<USDInputNodeWidget> out(new USDInputNodeWidget);
            out->_init(context, document, node, parent);
            return out;
        }

        render::NodeInfo USDInputNodeWidget::getClassNodeInfo()
        {
            return render::USDInputNode::getClassNodeInfo();
        }

        ftk::Size2I USDInputNodeWidget::getSizeHint() const
        {
            return _p->layout->getSizeHint();
        }

        void USDInputNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            INodeWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }
    }
}
