// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "NodeGraphCanvas.h"

#include "NodeDragDrop.h"
#include "NodeGraphWidget.h"

#include <ibis/Models/Document.h>

#include <ibis/Render/Graph.h>
#include <ibis/Render/GraphCmd.h>
#include <ibis/Render/NodeFactory.h>

namespace ibis
{
    namespace ui
    {
        struct NodeGraphCanvas::Private
        {
            std::shared_ptr<models::Document> document;
            std::shared_ptr<render::NodeFactory> nodeFactory;

            std::map<std::shared_ptr<render::INode>, std::shared_ptr<NodeGraphWidget> > nodeToWidget;
            std::map<std::shared_ptr<NodeGraphWidget>, std::shared_ptr<render::INode> > widgetToNode;
            std::map<std::shared_ptr<render::INode>, ftk::V2I> nodeToPos;

            int sizeHint = 0;
            int handle = 0;
            std::optional<Move> move;
            std::map<std::shared_ptr<render::INode>, ftk::V2I> moveNodes;
            std::optional<Connect> connect;
            bool dropTarget = false;

            std::shared_ptr<ftk::Observer<bool> > changedObserver;
            std::shared_ptr<ftk::ListObserver<std::shared_ptr<render::INode> > > selectionObserver;
            std::shared_ptr<ftk::Observer<std::shared_ptr<render::INode> > > viewNodeObserver;
        };

        void NodeGraphCanvas::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::NodeFactory>& nodeFactory,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::NodeGraphCanvas", parent);
            FTK_P();

            _setMouseHoverEnabled(true);
            _setMousePressEnabled(true);

            p.document = document;
            p.nodeFactory = nodeFactory;

            _graphUpdate();

            p.changedObserver = ftk::Observer<bool>::create(
                document->getGraph()->observe(),
                [this](bool value)
                {
                    if (value)
                    {
                        _graphUpdate();
                    }
                });

            p.selectionObserver = ftk::ListObserver<std::shared_ptr<render::INode> >::create(
                document->observeSelection(),
                [this](const std::vector<std::shared_ptr<render::INode> >& selection)
                {
                    FTK_P();

                    for (const auto i : p.nodeToWidget)
                    {
                        const auto j = std::find(selection.begin(), selection.end(), i.first);
                        i.second->setSelected(j != selection.end());
                    }

                    p.moveNodes.clear();
                    for (const auto& node : selection)
                    {
                        const auto i = p.nodeToPos.find(node);
                        if (i != p.nodeToPos.end())
                        {
                            p.moveNodes[node] = i->second;
                        }
                    }
                });

            p.viewNodeObserver = ftk::Observer<std::shared_ptr<render::INode> >::create(
                document->observeViewNode(),
                [this](const std::shared_ptr<render::INode>& node)
                {
                    FTK_P();
                    for (const auto& i : p.nodeToWidget)
                    {
                        i.second->setView(node == i.first);
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
            for (const auto i : p.nodeToWidget)
            {
                const ftk::Size2I sizeHint = i.second->getSizeHint();
                ftk::V2I pos;
                const auto j = p.nodeToPos.find(i.first);
                if (j != p.nodeToPos.end())
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
            p.handle = event.style->getSizeRole(ftk::SizeRole::Handle, event.displayScale);
        }

        void NodeGraphCanvas::drawOverlayEvent(
            const ftk::Box2I& drawRect,
            const ftk::DrawEvent& event)
        {
            IWidget::drawOverlayEvent(drawRect, event);
            FTK_P();
            const ftk::Box2I& g = getGeometry();
            if (p.connect)
            {
                // Draw in-progress connection.
                ftk::V2I v0;
                if (p.connect->input != -1)
                {
                    v0 = ftk::center(p.connect->widget->getInputs()[p.connect->input]->getGeometry());
                }
                else if (p.connect->output != -1)
                {
                    v0 = ftk::center(p.connect->widget->getOutputs()[p.connect->output]->getGeometry());
                }
                ftk::LineOptions lineOptions;
                lineOptions.width = p.handle / 2;
                event.render->drawLine(
                    v0,
                    _getMousePos(),
                    ftk::Color4F(1.F, 1.F, 1.F),
                    lineOptions);
            }
            else if (p.dropTarget)
            {
                event.render->drawRect(
                    g,
                    event.style->getColorRole(ftk::ColorRole::Overlay));
            }
        }

        void NodeGraphCanvas::drawEvent(
            const ftk::Box2I& drawRect,
            const ftk::DrawEvent& event)
        {
            IWidget::drawEvent(drawRect, event);
            FTK_P();
            const ftk::Box2I& g = getGeometry();
            for (const auto i : p.nodeToWidget)
            {
                // Draw connections.
                const auto& inputs = i.first->getInputs();
                for (int j = 0; j < inputs.size(); ++j)
                {
                    const auto& input = inputs[j];
                    if (input.node)
                    {
                        const auto k = p.nodeToWidget.find(input.node);
                        if (k != p.nodeToWidget.end())
                        {
                            const ftk::V2I v0 = ftk::center(i.second->getInputs()[j]->getGeometry());
                            const ftk::V2I v1 = ftk::center(k->second->getOutputs()[input.index]->getGeometry());
                            ftk::LineOptions lineOptions;
                            lineOptions.width = p.handle / 2;
                            event.render->drawLine(
                                v0,
                                v1,
                                ftk::Color4F(.5F, .5F, .5F),
                                lineOptions);
                        }
                    }
                }
            }
        }

        void NodeGraphCanvas::mouseMoveEvent(ftk::MouseMoveEvent& event)
        {
            IMouseWidget::mouseMoveEvent(event);
            FTK_P();
            if (p.move.has_value())
            {
                // Temporarily move the nodes.
                const ftk::V2I offset = event.pos - _getMousePressPos();
                for (const auto i : p.moveNodes)
                {
                    const auto j = p.nodeToPos.find(i.first);
                    if (j != p.nodeToPos.end())
                    {
                        j->second = i.second + offset;
                    }
                }
                setSizeUpdate();
                setDrawUpdate();
            }
            else if (p.connect.has_value())
            {
                setDrawUpdate();
            }
        }

        void NodeGraphCanvas::mousePressEvent(ftk::MouseClickEvent& event)
        {
            IMouseWidget::mousePressEvent(event);
            FTK_P();

            // Check for a connection.
            p.connect = _getConnect(event.pos);
            if (p.connect)
            {
                moveToFront(p.connect->widget);
            }
            else
            {
                // Check for a move.
                p.move = _getMove(event.pos);
                if (p.move)
                {
                    moveToFront(p.move->widget);
                    const auto& selection = p.document->getSelection();
                    const auto i = std::find(selection.begin(), selection.end(), p.move->node);
                    if (i == selection.end())
                    {
                        p.document->select({ p.move->node });
                    }
                    for (auto& i : p.moveNodes)
                    {
                        const auto j = p.nodeToPos.find(i.first);
                        if (j != p.nodeToPos.end())
                        {
                            i.second = j->second;
                        }
                    }
                }
                else
                {
                    p.document->clearSelection();
                }
            }
        }

        void NodeGraphCanvas::mouseReleaseEvent(ftk::MouseClickEvent& event)
        {
            IMouseWidget::mouseReleaseEvent(event);
            FTK_P();
            const auto& graph = p.document->getGraph();
            if (p.move.has_value())
            {
                // Move the nodes.
                const ftk::V2I offset = event.pos - _getMousePressPos();
                if (ftk::length(offset) > 0.F)
                {
                    std::vector<std::shared_ptr<render::INode> > nodes;
                    std::vector<ftk::V2I> pos;
                    for (const auto i : p.moveNodes)
                    {
                        nodes.push_back(i.first);
                        pos.push_back(i.second + offset);
                    }
                    p.document->command(
                        render::MoveNodesCmd::create(graph, nodes, pos));
                }
                p.move.reset();
            }
            else if (p.connect.has_value())
            {
                // Connect the nodes.
                if (p.connect->input != -1)
                {
                    const auto output = _getOutput(event.pos);
                    if (output.has_value())
                    {
                        p.document->command(
                            render::ConnectNodesCmd::create(
                                graph,
                                p.connect->node,
                                p.connect->input,
                                output->node,
                                output->output));
                    }
                }
                else if (p.connect->output != -1)
                {
                    const auto input = _getInput(event.pos);
                    if (input.has_value())
                    {
                        p.document->command(
                            render::ConnectNodesCmd::create(
                                graph,
                                input->node,
                                input->input,
                                p.connect->node,
                                p.connect->output));
                    }
                }
                p.connect.reset();
                setDrawUpdate();
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
                    p.document->command(
                        render::AddNodesCmd::create(
                            p.document->getGraph(),
                            { node },
                            { event.pos - g.min }));
                }
                setDrawUpdate();
            }
        }

        std::optional<NodeGraphCanvas::Move> NodeGraphCanvas::_getMove(const ftk::V2I& pos)
        {
            FTK_P();
            std::optional<NodeGraphCanvas::Move> out;
            const auto& children = getChildren();
            for (auto i = children.rbegin(); i != children.rend(); ++i)
            {
                if (auto widget = std::dynamic_pointer_cast<NodeGraphWidget>(*i))
                {
                    const ftk::Box2I& g = widget->getGeometry();
                    if (ftk::contains(g, pos))
                    {
                        out = Move();
                        out->node = widget->getNode();
                        out->widget = widget;
                        break;
                    }
                }
            }
            return out;
        }

        std::optional<NodeGraphCanvas::Connect> NodeGraphCanvas::_getConnect(const ftk::V2I& pos)
        {
            std::optional<NodeGraphCanvas::Connect> out;
            const auto& children = getChildren();
            for (auto i = children.rbegin(); i != children.rend(); ++i)
            {
                if (auto widget = std::dynamic_pointer_cast<NodeGraphWidget>(*i))
                {
                    const auto& inputs = widget->getInputs();
                    for (int i = 0; i < inputs.size(); ++i)
                    {
                        const ftk::Box2I& g = inputs[i]->getGeometry();
                        if (ftk::contains(g, pos))
                        {
                            out = Connect();
                            out->node = widget->getNode();
                            out->widget = widget;
                            out->input = i;
                            break;
                        }
                    }
                    const auto& outputs = widget->getOutputs();
                    for (int i = 0; i < outputs.size(); ++i)
                    {
                        const ftk::Box2I& g = outputs[i]->getGeometry();
                        if (ftk::contains(g, pos))
                        {
                            out = Connect();
                            out->node = widget->getNode();
                            out->widget = widget;
                            out->output = i;
                            break;
                        }
                    }
                }
            }
            return out;
        }

        std::optional<NodeGraphCanvas::Input> NodeGraphCanvas::_getInput(const ftk::V2I& pos)
        {
            std::optional<NodeGraphCanvas::Input> out;
            const auto& children = getChildren();
            for (auto i = children.rbegin(); i != children.rend(); ++i)
            {
                if (auto widget = std::dynamic_pointer_cast<NodeGraphWidget>(*i))
                {
                    const auto& inputs = widget->getInputs();
                    for (int i = 0; i < inputs.size(); ++i)
                    {
                        const ftk::Box2I& g = inputs[i]->getGeometry();
                        if (ftk::contains(g, pos))
                        {
                            out = Input();
                            out->node = widget->getNode();
                            out->widget = widget;
                            out->input = i;
                            break;
                        }
                    }
                }
            }
            return out;
        }

        std::optional<NodeGraphCanvas::Output> NodeGraphCanvas::_getOutput(const ftk::V2I& pos)
        {
            std::optional<NodeGraphCanvas::Output> out;
            const auto& children = getChildren();
            for (auto i = children.rbegin(); i != children.rend(); ++i)
            {
                if (auto widget = std::dynamic_pointer_cast<NodeGraphWidget>(*i))
                {
                    const auto& outputs = widget->getOutputs();
                    for (int i = 0; i < outputs.size(); ++i)
                    {
                        const ftk::Box2I& g = outputs[i]->getGeometry();
                        if (ftk::contains(g, pos))
                        {
                            out = Output();
                            out->node = widget->getNode();
                            out->widget = widget;
                            out->output = i;
                            break;
                        }
                    }
                }
            }
            return out;
        }

        void NodeGraphCanvas::_graphUpdate()
        {
            FTK_P();
            std::map<std::shared_ptr<render::INode>, std::shared_ptr<NodeGraphWidget> > nodeToWidget;
            std::map<std::shared_ptr<NodeGraphWidget>, std::shared_ptr<render::INode> > widgetToNode;
            std::map<std::shared_ptr<render::INode>, ftk::V2I> nodeToPos;

            // Create new widgets.
            const auto& graph = p.document->getGraph();
            for (const auto& node : graph->getNodes())
            {
                std::shared_ptr<NodeGraphWidget> widget;
                const auto j = p.nodeToWidget.find(node);
                if (j == p.nodeToWidget.end())
                {
                    widget = NodeGraphWidget::create(getContext(), node, shared_from_this());
                    widget->setView(node == p.document->getViewNode());
                    widget->setViewCallback(
                        [this](const std::shared_ptr<render::INode>& node)
                        {
                            _p->document->setViewNode(node);
                        });
                }
                else
                {
                    widget = j->second;
                }
                nodeToWidget[node] = widget;
                widgetToNode[widget] = node;
                nodeToPos[node] = graph->getPos(node);
            }

            // Remove old widgets.
            for (const auto i : p.nodeToWidget)
            {
                const auto j = nodeToWidget.find(i.first);
                if (j == nodeToWidget.end())
                {
                    i.second->setParent(nullptr);
                }
            }

            p.nodeToWidget = nodeToWidget;
            p.widgetToNode = widgetToNode;
            p.nodeToPos = nodeToPos;
            setSizeUpdate();
            setDrawUpdate();
        }
    }
}
