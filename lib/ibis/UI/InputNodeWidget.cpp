// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "InputNodeWidget.h"

#include <ibis/Models/Document.h>

#include <ibis/Render/Graph.h>
#include <ibis/Render/GraphCmd.h>
#include <ibis/Render/InputNode.h>

#include <ftk/UI/Icon.h>
#include <ftk/UI/FileEdit.h>
#include <ftk/UI/FormLayout.h>
#include <ftk/UI/RowLayout.h>

#include <ftk/Core/Format.h>
#include <ftk/Core/String.h>

namespace ibis
{
    namespace ui
    {
        struct SVGInputNodeWidget::Private
        {
            std::shared_ptr<ftk::FileEdit> fileEdit;
            std::shared_ptr<ftk::FormLayout> layout;

            std::shared_ptr<ftk::MapObserver<std::string, nlohmann::json> > observer;
        };

        void SVGInputNodeWidget::_init(
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
                arg(ftk::join(render::SVGInputNode::getExts(), ", ")));

            p.layout = ftk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(ftk::SizeRole::Margin);
            auto hLayout = ftk::HorizontalLayout::create(context);
            hLayout->setSpacingRole(ftk::SizeRole::SpacingTool);
            hLayout->setHStretch(ftk::Stretch::Expanding);
            p.fileEdit->setParent(hLayout);
            infoIcon->setParent(hLayout);
            p.layout->addRow("Path:", hLayout);

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

            p.observer = ftk::MapObserver<std::string, nlohmann::json>::create(
                node->observeAttr(),
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

        SVGInputNodeWidget::SVGInputNodeWidget() :
            _p(new Private)
        {}

        SVGInputNodeWidget::~SVGInputNodeWidget()
        {}

        std::shared_ptr<SVGInputNodeWidget> SVGInputNodeWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<SVGInputNodeWidget> out(new SVGInputNodeWidget);
            out->_init(context, document, node, parent);
            return out;
        }

        render::NodeInfo SVGInputNodeWidget::getClassNodeInfo()
        {
            return render::SVGInputNode::getClassNodeInfo();
        }

        ftk::Size2I SVGInputNodeWidget::getSizeHint() const
        {
            return _p->layout->getSizeHint();
        }

        void SVGInputNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            INodeWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }
    }
}
