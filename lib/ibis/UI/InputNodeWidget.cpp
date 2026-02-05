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
        struct SVGFileNodeWidget::Private
        {
            std::shared_ptr<ftk::FileEdit> fileEdit;
            std::shared_ptr<ftk::Bellows> bellows;

            std::shared_ptr<ftk::MapObserver<std::string, nlohmann::json> > observer;
        };

        void SVGFileNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            INodeWidget::_init(context, document, node, parent);
            FTK_P();

            p.fileEdit = ftk::FileEdit::create(context);

            auto formLayout = ftk::FormLayout::create(context);
            formLayout->setMarginRole(ftk::SizeRole::Margin);
            formLayout->addRow("Path:", p.fileEdit);
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

        SVGFileNodeWidget::SVGFileNodeWidget() :
            _p(new Private)
        {}

        SVGFileNodeWidget::~SVGFileNodeWidget()
        {}

        std::shared_ptr<SVGFileNodeWidget> SVGFileNodeWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<SVGFileNodeWidget> out(new SVGFileNodeWidget);
            out->_init(context, document, node, parent);
            return out;
        }

        render::NodeInfo SVGFileNodeWidget::getClassNodeInfo()
        {
            return render::SVGFileNode::getClassNodeInfo();
        }

        ftk::Size2I SVGFileNodeWidget::getSizeHint() const
        {
            return _p->bellows->getSizeHint();
        }

        void SVGFileNodeWidget::setGeometry(const ftk::Box2I& value)
        {
            INodeWidget::setGeometry(value);
            _p->bellows->setGeometry(value);
        }
    }
}
