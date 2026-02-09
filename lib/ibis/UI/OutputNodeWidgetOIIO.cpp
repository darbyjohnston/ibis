// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "OutputNodeWidget.h"

#include "InputNodeWidget.h"

#include <ibis/Models/Document.h>

#include <ibis/Render/Graph.h>
#include <ibis/Render/GraphCmd.h>
#include <ibis/Render/OutputNode.h>

#include <ftk/UI/Bellows.h>
#include <ftk/UI/ComboBox.h>
#include <ftk/UI/FileEdit.h>
#include <ftk/UI/Icon.h>
#include <ftk/UI/FormLayout.h>
#include <ftk/UI/RowLayout.h>

#include <ftk/Core/Format.h>
#include <ftk/Core/String.h>

namespace ibis
{
    namespace ui
    {
        struct ImageOutputNodeWidget::Private
        {
            std::shared_ptr<ftk::FileEdit> fileEdit;
            std::shared_ptr<ftk::Bellows> bellows;

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

            p.fileEdit = ftk::FileEdit::create(context);

            auto infoIcon = ftk::Icon::create(context, "Info");
            infoIcon->setTooltip(
                ftk::Format("Supported file extensions:\n{0}").
                arg(getExtTooltip(render::ImageOutputNode::getExts())));

            auto formLayout = ftk::FormLayout::create(context);
            formLayout->setMarginRole(ftk::SizeRole::Margin);
            auto hLayout = ftk::HorizontalLayout::create(context);
            hLayout->setSpacingRole(ftk::SizeRole::SpacingTool);
            hLayout->setHStretch(ftk::Stretch::Expanding);
            p.fileEdit->setParent(hLayout);
            infoIcon->setParent(hLayout);
            formLayout->addRow("Path:", hLayout);
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

            p.attrObserver = ftk::MapObserver<std::string, nlohmann::json>::create(
                node->observeAttr(),
                [this](const std::map<std::string, nlohmann::json>& value)
                {
                    FTK_P();
                    auto tmp = value;
                    p.fileEdit->setPath(ftk::Path(tmp["Path"]));
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
            return _p->bellows->getSizeHint();
        }

        void ImageOutputNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            INodeWidget::setGeometry(value);
            _p->bellows->setGeometry(value);
        }
    }
}
