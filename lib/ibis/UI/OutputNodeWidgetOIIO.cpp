// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "OutputNodeWidget.h"

#include "InputNodeWidget.h"

#include <ibis/Models/Document.h>

#include <ibis/Render/Graph.h>
#include <ibis/Render/GraphCmd.h>
#include <ibis/Render/OutputNode.h>

#include <ftk/UI/ComboBox.h>
#include <ftk/UI/FileEdit.h>
#include <ftk/UI/FormLayout.h>
#include <ftk/UI/IntEdit.h>
#include <ftk/UI/LineEdit.h>
#include <ftk/UI/RowLayout.h>
#include <ftk/Core/Format.h>
#include <ftk/Core/String.h>

namespace ibis
{
    namespace ui
    {
        struct ImageOutputNodeWidget::Private
        {
            std::shared_ptr<ftk::FileEdit> dirEdit;
            std::shared_ptr<ftk::LineEdit> baseNameEdit;
            std::shared_ptr<ftk::ComboBox> extComboBox;
            std::shared_ptr<ftk::FormLayout> layout;

            std::shared_ptr<ftk::MapObserver<std::string, nlohmann::json> > attrObserver;
        };

        void ImageOutputNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            INodeWidget::_init(context, document, node, parent);
            FTK_P();

            p.dirEdit = ftk::FileEdit::create(context, ftk::FileBrowserMode::Dir);

            p.baseNameEdit = ftk::LineEdit::create(context);
            p.baseNameEdit->setHStretch(ftk::Stretch::Expanding);

            p.extComboBox = ftk::ComboBox::create(context, render::ImageOutputNode::getExts());

            p.layout = ftk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(ftk::SizeRole::Margin);
            p.layout->addRow("Directory:", p.dirEdit);
            p.layout->addRow("Base name:", p.baseNameEdit);
            p.layout->addRow("Extension:", p.extComboBox);

            p.dirEdit->setCallback(
                [this](const ftk::Path& value)
                {
                    // Check that the path has actually changed, the callback
                    // can also be triggered by focus changes.
                    if (value.get() != _node->getAttr("Dir"))
                    {
                        _document->command(render::NodeAttrCmd::create(
                            _document->getGraph(),
                            _node,
                            "Dir",
                            value.get()));
                    }
                });

            p.baseNameEdit->setCallback(
                [this](const std::string& value)
                {
                    // Check that the path has actually changed, the callback
                    // can also be triggered by focus changes.
                    if (value != _node->getAttr("BaseName"))
                    {
                        _document->command(render::NodeAttrCmd::create(
                            _document->getGraph(),
                            _node,
                            "BaseName",
                            value));
                    }
                });

            p.extComboBox->setIndexCallback(
                [this](int value)
                {
                    const auto exts = render::ImageOutputNode::getExts();
                    if (value >= 0 && value < exts.size())
                    {
                        _document->command(render::NodeAttrCmd::create(
                            _document->getGraph(),
                            _node,
                            "Ext",
                            exts[value]));
                    }
                });

            p.attrObserver = ftk::MapObserver<std::string, nlohmann::json>::create(
                node->observeAttr(),
                [this](const std::map<std::string, nlohmann::json>& value)
                {
                    FTK_P();
                    auto tmp = value;
                    p.dirEdit->setPath(ftk::Path(tmp["Dir"]));
                    p.baseNameEdit->setText(tmp["BaseName"]);
                    const auto exts = render::ImageOutputNode::getExts();
                    const auto i = std::find(exts.begin(), exts.end(), tmp["Ext"]);
                    p.extComboBox->setCurrentIndex(i != exts.end() ? i - exts.begin() : -1);
                });
        }

        ImageOutputNodeWidget::ImageOutputNodeWidget() :
            _p(new Private)
        {}

        ImageOutputNodeWidget::~ImageOutputNodeWidget()
        {}

        std::shared_ptr<ImageOutputNodeWidget> ImageOutputNodeWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<ImageOutputNodeWidget> out(new ImageOutputNodeWidget);
            out->_init(context, document, node, parent);
            return out;
        }

        render::NodeInfo ImageOutputNodeWidget::getClassNodeInfo()
        {
            return render::ImageOutputNode::getClassNodeInfo();
        }

        ftk::Size2I ImageOutputNodeWidget::getSizeHint() const
        {
            return _p->layout->getSizeHint();
        }

        void ImageOutputNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            INodeWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        struct SequenceOutputNodeWidget::Private
        {
            std::shared_ptr<ftk::FileEdit> dirEdit;
            std::shared_ptr<ftk::LineEdit> baseNameEdit;
            std::shared_ptr<ftk::IntEdit> padEdit;
            std::shared_ptr<ftk::ComboBox> extComboBox;
            std::shared_ptr<ftk::FormLayout> layout;

            std::shared_ptr<ftk::MapObserver<std::string, nlohmann::json> > attrObserver;
        };

        void SequenceOutputNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            INodeWidget::_init(context, document, node, parent);
            FTK_P();

            p.dirEdit = ftk::FileEdit::create(context, ftk::FileBrowserMode::Dir);

            p.baseNameEdit = ftk::LineEdit::create(context);
            p.baseNameEdit->setHStretch(ftk::Stretch::Expanding);

            p.padEdit = ftk::IntEdit::create(context);
            p.padEdit->setRange(0, 9);

            p.extComboBox = ftk::ComboBox::create(context, render::ImageOutputNode::getExts());

            p.layout = ftk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(ftk::SizeRole::Margin);
            p.layout->addRow("Directory:", p.dirEdit);
            p.layout->addRow("Base name:", p.baseNameEdit);
            p.layout->addRow("Zero padding:", p.padEdit);
            p.layout->addRow("Extension:", p.extComboBox);

            p.dirEdit->setCallback(
                [this](const ftk::Path& value)
                {
                    // Check that the path has actually changed, the callback
                    // can also be triggered by focus changes.
                    if (value.get() != _node->getAttr("Dir"))
                    {
                        _document->command(render::NodeAttrCmd::create(
                            _document->getGraph(),
                            _node,
                            "Dir",
                            value.get()));
                    }
                });

            p.baseNameEdit->setCallback(
                [this](const std::string& value)
                {
                    // Check that the path has actually changed, the callback
                    // can also be triggered by focus changes.
                    if (value != _node->getAttr("BaseName"))
                    {
                        _document->command(render::NodeAttrCmd::create(
                            _document->getGraph(),
                            _node,
                            "BaseName",
                            value));
                    }
                });

            p.padEdit->setCallback(
                [this](int value)
                {
                    _document->command(render::NodeAttrCmd::create(
                        _document->getGraph(),
                        _node,
                        "Pad",
                        value));
                });

            p.extComboBox->setIndexCallback(
                [this](int value)
                {
                    const auto exts = render::ImageOutputNode::getExts();
                    if (value >= 0 && value < exts.size())
                    {
                        _document->command(render::NodeAttrCmd::create(
                            _document->getGraph(),
                            _node,
                            "Ext",
                            exts[value]));
                    }
                });

            p.attrObserver = ftk::MapObserver<std::string, nlohmann::json>::create(
                node->observeAttr(),
                [this](const std::map<std::string, nlohmann::json>& value)
                {
                    FTK_P();
                    auto tmp = value;
                    p.dirEdit->setPath(ftk::Path(tmp["Dir"]));
                    p.baseNameEdit->setText(tmp["BaseName"]);
                    p.padEdit->setValue(tmp["Pad"]);
                    const auto exts = render::ImageOutputNode::getExts();
                    const auto i = std::find(exts.begin(), exts.end(), tmp["Ext"]);
                    p.extComboBox->setCurrentIndex(i != exts.end() ? i - exts.begin() : -1);
                });
        }

        SequenceOutputNodeWidget::SequenceOutputNodeWidget() :
            _p(new Private)
        {}

        SequenceOutputNodeWidget::~SequenceOutputNodeWidget()
        {}

        std::shared_ptr<SequenceOutputNodeWidget> SequenceOutputNodeWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<SequenceOutputNodeWidget> out(new SequenceOutputNodeWidget);
            out->_init(context, document, node, parent);
            return out;
        }

        render::NodeInfo SequenceOutputNodeWidget::getClassNodeInfo()
        {
            return render::SequenceOutputNode::getClassNodeInfo();
        }

        ftk::Size2I SequenceOutputNodeWidget::getSizeHint() const
        {
            return _p->layout->getSizeHint();
        }

        void SequenceOutputNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            INodeWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }
    }
}
