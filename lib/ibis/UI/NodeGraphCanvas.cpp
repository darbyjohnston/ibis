// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "NodeGraphCanvas.h"

#include "NodeDragDrop.h"
#include "NodeGraphWidget.h"

#include <ibis/Models/Document.h>

#include <ibis/Render/Graph.h>
#include <ibis/Render/GraphCommands.h>
#include <ibis/Render/NodeFactory.h>

namespace ibis
{
    namespace ui
    {
        struct NodeGraphCanvas::Private
        {
            std::shared_ptr<models::Document> document;
            std::shared_ptr<render::NodeFactory> nodeFactory;
            std::map<std::shared_ptr<render::INode>, std::shared_ptr<NodeGraphWidget> > widgets;
            std::map<std::shared_ptr<render::INode>, ftk::V2I> pos;
            int sizeHint = 0;
            bool dropTarget = false;
            std::shared_ptr<ftk::Observer<bool> > changedObserver;
        };

        void NodeGraphCanvas::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::NodeFactory>& nodeFactory,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::NodeGraphCanvas", parent);
            FTK_P();

            p.document = document;
            p.nodeFactory = nodeFactory;

            p.changedObserver = ftk::Observer<bool>::create(
                document->getGraph()->observe(),
                [this](bool value)
                {
                    if (value)
                    {
                        _graphUpdate();
                    }
                });
        }

        NodeGraphCanvas::NodeGraphCanvas() :
            _p(new Private)
        {}

        NodeGraphCanvas::~NodeGraphCanvas()
        {}

        std::shared_ptr<NodeGraphCanvas> NodeGraphCanvas::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::NodeFactory>& nodeFactory,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<NodeGraphCanvas> out(new NodeGraphCanvas);
            out->_init(context, document, nodeFactory, parent);
            return out;
        }

        ftk::Size2I NodeGraphCanvas::getSizeHint() const
        {
            return ftk::Size2I(_p->sizeHint, _p->sizeHint);
        }

        void NodeGraphCanvas::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            FTK_P();
            for (const auto i : p.widgets)
            {
                const ftk::Size2I sizeHint = i.second->getSizeHint();
                ftk::V2I pos;
                const auto j = p.pos.find(i.first);
                if (j != p.pos.end())
                {
                    pos = j->second;
                }
                i.second->setGeometry(ftk::Box2I(
                    value.min.x + pos.x,
                    value.min.y + pos.y,
                    sizeHint.w,
                    sizeHint.h));
            }
        }

        void NodeGraphCanvas::sizeHintEvent(const ftk::SizeHintEvent& event)
        {
            FTK_P();
            p.sizeHint = event.style->getSizeRole(ftk::SizeRole::ScrollArea, event.displayScale);
        }

        void NodeGraphCanvas::drawOverlayEvent(
            const ftk::Box2I& drawRect,
            const ftk::DrawEvent& event)
        {
            IWidget::drawOverlayEvent(drawRect, event);
            FTK_P();
            if (p.dropTarget)
            {
                const ftk::Box2I& g = getGeometry();
                event.render->drawRect(
                    g,
                    event.style->getColorRole(ftk::ColorRole::Overlay));
            }
        }

        void NodeGraphCanvas::dragEnterEvent(ftk::DragDropEvent& event)
        {
            FTK_P();
            if (std::dynamic_pointer_cast<NodeDragDropData>(event.data))
            {
                event.accept = true;
                p.dropTarget = true;
                setDrawUpdate();
            }
        }

        void NodeGraphCanvas::dragLeaveEvent(ftk::DragDropEvent& event)
        {
            FTK_P();
            if (p.dropTarget)
            {
                event.accept = true;
                p.dropTarget = false;
                setDrawUpdate();
            }
        }

        void NodeGraphCanvas::dragMoveEvent(ftk::DragDropEvent& event)
        {
            event.accept = true;
        }

        void NodeGraphCanvas::dropEvent(ftk::DragDropEvent& event)
        {
            FTK_P();
            if (auto data = std::dynamic_pointer_cast<NodeDragDropData>(event.data))
            {
                event.accept = true;
                p.dropTarget = false;
                if (auto node = p.nodeFactory->createNode(data->getNode()))
                {
                    const ftk::Box2I& g = getGeometry();
                    p.document->getCommandStack()->push(
                        render::AddNodeCommand::create(
                            p.document->getGraph(),
                            node,
                            event.pos - g.min));
                }
                setDrawUpdate();
            }
        }

        void NodeGraphCanvas::_graphUpdate()
        {
            FTK_P();
            std::map<std::shared_ptr<render::INode>, std::shared_ptr<NodeGraphWidget> > widgets;
            std::map<std::shared_ptr<render::INode>, ftk::V2I> pos;
            auto graph = p.document->getGraph();
            const auto& nodes = graph->getNodes();
            for (const auto& node : nodes)
            {
                const auto j = p.widgets.find(node);
                if (j == p.widgets.end())
                {
                    auto widget = NodeGraphWidget::create(getContext(), shared_from_this());
                    widgets[node] = widget;
                }
                else
                {
                    widgets[node] = j->second;
                }
                pos[node] = graph->getPos(node);
            }
            for (const auto i : p.widgets)
            {
                const auto j = widgets.find(i.first);
                if (j == widgets.end())
                {
                    i.second->setParent(nullptr);
                }
            }
            p.widgets = widgets;
            p.pos = pos;
            setDrawUpdate();
        }
    }
}
