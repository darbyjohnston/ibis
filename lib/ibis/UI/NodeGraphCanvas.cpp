// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "NodeGraphCanvas.h"

#include "NodeDragDrop.h"
#include "NodeGraphWidget.h"

#include <ibis/Models/Document.h>

#include <ibis/Render/Graph.h>
#include <ibis/Render/GraphCmd.h>
#include <ibis/Render/NodeFactory.h>

#include <ftk/UI/DrawUtil.h>
#include <ftk/UI/Menu.h>
#include <ftk/UI/ScrollWidget.h>
#include <ftk/Core/Timer.h>

namespace ibis
{
    namespace ui
    {
        struct NodeGraphCanvas::Private
        {
            std::shared_ptr<models::Document> document;
            std::shared_ptr<render::NodeFactory> nodeFactory;
            std::map<std::string, std::shared_ptr<ftk::Action> > editActions;
            ftk::Size2I canvasSize = ftk::Size2I(2000, 2000);
            bool grid = true;
            int gridSize = 100;
            std::function<void(const std::vector<ftk::Box2I>&)> childGeometryCallback;
            std::vector<ftk::Box2I> childGeomPrev;

            std::map<std::shared_ptr<render::INode>, std::shared_ptr<NodeGraphWidget> > nodeToWidget;
            std::map<std::shared_ptr<NodeGraphWidget>, std::shared_ptr<render::INode> > widgetToNode;
            std::map<std::shared_ptr<render::INode>, ftk::V2I> nodeToPos;
            bool viewInit = true;
            std::shared_ptr<ftk::Menu> menu;

            std::shared_ptr<ftk::Observer<bool> > changedObserver;
            std::shared_ptr<ftk::ListObserver<std::shared_ptr<render::INode> > > selectionObserver;
            std::shared_ptr<ftk::Observer<std::shared_ptr<render::INode> > > viewNodeObserver;
            std::shared_ptr<ftk::Observer<ftk::Size2I> > canvasSizeObserver;
            std::shared_ptr<ftk::Observer<bool> > gridObserver;
            std::shared_ptr<ftk::Observer<int> > gridSizeObserver;

            struct SizeData
            {
                ftk::Size2I canvas;
                int grid = 0;
                int margin = 0;
                int border = 0;
                int handle = 0;
                int drag = 0;
                int shadow = 0;
            };
            SizeData size;

            enum class MouseMode
            {
                None,
                MoveNodes,
                ConnectNodes,
                Pan,
                Select
            };
            struct MouseData
            {
                MouseMode mode = MouseMode::None;

                // Whether the mouse is inside the widget.
                bool inside = false;

                // Current mouse position.
                ftk::V2I pos;

                // Mouse press position in regular and canvas coordinates.
                bool pressed = false;
                ftk::V2I press;
                ftk::V2I canvasPress;

                // Data for moving nodes.
                std::optional<Move> move;
                std::vector<std::shared_ptr<render::INode> > moveNodes;

                // Data for connecting nodes.
                std::optional<Connect> connect;
                std::shared_ptr<INodeGraphPort> connectPort;

                // Auto-scroll timer.
                std::shared_ptr<ftk::Timer> autoScrollTimer;
            };
            MouseData mouse;

            bool dropTarget = false;
        };

        void NodeGraphCanvas::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<render::NodeFactory>& nodeFactory,
            const std::shared_ptr<models::Document>& document,
            const std::map<std::string, std::shared_ptr<ftk::Action> >& editActions,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::NodeGraphCanvas", parent);
            FTK_P();

            setAcceptsKeyFocus(true);

            p.document = document;
            p.nodeFactory = nodeFactory;
            p.editActions = editActions;

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
                    for (const auto& i : p.nodeToWidget)
                    {
                        const auto j = std::find(selection.begin(), selection.end(), i.first);
                        i.second->setSelected(j != selection.end());
                    }
                    p.mouse.moveNodes = selection;
                });

            p.viewNodeObserver = ftk::Observer<std::shared_ptr<render::INode> >::create(
                document->observeViewNode(),
                [this](const std::shared_ptr<render::INode>& node)
                {
                    FTK_P();
                    for (const auto& i : p.nodeToWidget)
                    {
                        i.second->setView(i.first == node);
                    }
                });

            p.canvasSizeObserver = ftk::Observer<ftk::Size2I>::create(
                document->observeCanvasSize(),
                [this](const ftk::Size2I& value)
                {
                    FTK_P();
                    p.canvasSize = value;
                    setSizeUpdate();
                    setDrawUpdate();
                });

            p.gridObserver = ftk::Observer<bool>::create(
                document->observeGrid(),
                [this](bool value)
                {
                    FTK_P();
                    p.grid = value;
                    setDrawUpdate();
                });

            p.gridSizeObserver = ftk::Observer<int>::create(
                document->observeGridSize(),
                [this](int value)
                {
                    FTK_P();
                    p.gridSize = value;
                    setSizeUpdate();
                    setDrawUpdate();
                });
        }

        NodeGraphCanvas::NodeGraphCanvas() :
            _p(new Private)
        {}

        NodeGraphCanvas::~NodeGraphCanvas()
        {}

        std::shared_ptr<NodeGraphCanvas> NodeGraphCanvas::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<render::NodeFactory>& nodeFactory,
            const std::shared_ptr<models::Document>& document,
            const std::map<std::string, std::shared_ptr<ftk::Action> >& editActions,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<NodeGraphCanvas> out(new NodeGraphCanvas);
            out->_init(context, nodeFactory, document, editActions, parent);
            return out;
        }

        void NodeGraphCanvas::scrollTo(const std::shared_ptr<render::INode>& node)
        {
            FTK_P();
            std::shared_ptr<NodeGraphWidget> widget;
            const auto i = p.nodeToWidget.find(node);
            if (i != p.nodeToWidget.end())
            {
                widget = i->second;
            }
            if (node && widget)
            {
                const ftk::Box2I& widgetGeom = widget->getGeometry();
                ftk::V2I pos;
                const auto i = p.nodeToPos.find(node);
                if (i != p.nodeToPos.end())
                {
                    pos = pos + i->second + ftk::V2I(widgetGeom.w() / 2, widgetGeom.h() / 2);
                }
                auto scrollWidget = getParentT<ftk::ScrollWidget>();
                const ftk::Box2I& scrollGeom = scrollWidget->getScrollArea()->getGeometry();
                pos.x -= scrollGeom.w() / 2;
                pos.y -= scrollGeom.h() / 2;
                scrollWidget->setScrollPos(pos);
            }
        }

        void NodeGraphCanvas::setChildGeometryCallback(
            const std::function<void(const std::vector<ftk::Box2I>&)>& value)
        {
            _p->childGeometryCallback = value;
        }

        ftk::Size2I NodeGraphCanvas::getSizeHint() const
        {
            return _p->size.canvas;
        }

        void NodeGraphCanvas::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            FTK_P();

            std::vector<ftk::Box2I> childGeomList;
            for (const auto i : p.nodeToWidget)
            {
                const ftk::Size2I sizeHint = i.second->getSizeHint();
                ftk::V2I pos;
                const auto j = p.nodeToPos.find(i.first);
                if (j != p.nodeToPos.end())
                {
                    j->second.x = ftk::clamp(j->second.x, 0, value.w() - sizeHint.w);
                    j->second.y = ftk::clamp(j->second.y, 0, value.h() - sizeHint.h);
                    pos = j->second;
                }
                i.second->setGeometry(ftk::Box2I(pos + value.min, sizeHint));
                childGeomList.push_back(ftk::Box2I(pos, sizeHint));
            }
            if (childGeomList != p.childGeomPrev && p.childGeometryCallback)
            {
                p.childGeometryCallback(childGeomList);
            }
            p.childGeomPrev = childGeomList;

            if (p.viewInit)
            {
                auto scrollWidget = getParentT<ftk::ScrollWidget>();
                if (scrollWidget->getScrollSize().isValid())
                {
                    p.viewInit = false;
                    std::shared_ptr<render::INode> node;
                    if (!p.nodeToWidget.empty())
                    {
                        node = p.nodeToWidget.begin()->first;
                    }
                    scrollTo(node);
                }
            }
        }

        void NodeGraphCanvas::sizeHintEvent(const ftk::SizeHintEvent& event)
        {
            FTK_P();
            p.size.canvas = p.canvasSize * event.displayScale;
            p.size.grid = p.gridSize * event.displayScale;
            p.size.margin = event.style->getSizeRole(ftk::SizeRole::MarginLarge, event.displayScale);
            p.size.border = event.style->getSizeRole(ftk::SizeRole::Border, event.displayScale);
            p.size.handle = event.style->getSizeRole(ftk::SizeRole::Handle, event.displayScale);
            p.size.drag = event.style->getSizeRole(ftk::SizeRole::DragLength, event.displayScale);
            p.size.shadow = event.style->getSizeRole(ftk::SizeRole::Shadow, event.displayScale);
        }

        void NodeGraphCanvas::drawOverlayEvent(
            const ftk::Box2I& drawRect,
            const ftk::DrawEvent& event)
        {
            IWidget::drawOverlayEvent(drawRect, event);
            FTK_P();
            const ftk::Box2I& g = getGeometry();
            switch (p.mouse.mode)
            {
            case Private::MouseMode::ConnectNodes:
                if (p.mouse.connect)
                {
                    // Draw the in-progress connection.
                    ftk::V2I v0;
                    if (p.mouse.connect->input != -1)
                    {
                        v0 = ftk::center(p.mouse.connect->widget->getInputs()[p.mouse.connect->input]->getGeometry());
                    }
                    else if (p.mouse.connect->output != -1)
                    {
                        v0 = ftk::center(p.mouse.connect->widget->getOutputs()[p.mouse.connect->output]->getGeometry());
                    }
                    ftk::V2I v1 = p.mouse.pos;
                    ftk::LineOptions lineOptions;
                    lineOptions.width = p.size.handle / 2;
                    event.render->drawLine(
                        v0,
                        v1,
                        ftk::Color4F(1.F, 1.F, 1.F),
                        lineOptions);
                    event.render->drawMesh(ftk::circle(v0, p.size.handle / 2));
                    event.render->drawMesh(ftk::circle(v1, p.size.handle / 2));
                }
                break;

            case Private::MouseMode::Select:
            {
                // Draw the selection rectangle.
                ftk::Color4F c = event.style->getColorRole(ftk::ColorRole::Checked);
                c.a = .5F;
                event.render->drawRect(_getSelectionRect(), c);
                break;
            }

            default: break;
            }
        }

        void NodeGraphCanvas::drawEvent(
            const ftk::Box2I& drawRect,
            const ftk::DrawEvent& event)
        {
            IWidget::drawEvent(drawRect, event);
            FTK_P();
            const ftk::Box2I& g = getGeometry();

            // Draw the grid.
            if (p.grid)
            {
                std::vector<std::pair<ftk::V2I, ftk::V2I> > lines;
                for (int y = p.size.grid; y < p.size.canvas.h; y += p.size.grid)
                {
                    const int y2 = g.min.y + y;
                    if (y2 >= drawRect.min.y && y2 <= drawRect.max.y)
                    {
                        lines.push_back(std::make_pair(
                            ftk::V2I(g.min.x, g.min.y + y),
                            ftk::V2I(g.max.x, g.min.y + y)));
                    }
                }
                for (int x = p.size.grid; x < p.size.canvas.w; x += p.size.grid)
                {
                    const int x2 = g.min.x + x;
                    if (x2 >= drawRect.min.x && x2 <= drawRect.max.x)
                    {
                        lines.push_back(std::make_pair(
                            ftk::V2I(g.min.x + x, g.min.y),
                            ftk::V2I(g.min.x + x, g.max.y)));
                    }
                }
                event.render->drawLines(lines, event.style->getColorRole(ftk::ColorRole::Border));
            }

            // Draw shadows.
            for (const auto i : p.nodeToWidget)
            {
                const ftk::Box2I& g2 = i.second->getGeometry();
                event.render->drawColorMesh(ftk::shadow(
                    ftk::margin(g2, p.size.shadow, 0, p.size.shadow, p.size.shadow),
                    p.size.shadow));
            }

            // Draw connections.
            for (const auto i : p.nodeToWidget)
            {
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
                            lineOptions.width = p.size.handle / 2;
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

        void NodeGraphCanvas::mouseEnterEvent(ftk::MouseEnterEvent& event)
        {
            FTK_P();
            event.accept = true;
            p.mouse.inside = true;
        }

        void NodeGraphCanvas::mouseLeaveEvent()
        {
            FTK_P();
            p.mouse.inside = false;
        }

        void NodeGraphCanvas::mouseMoveEvent(ftk::MouseMoveEvent& event)
        {
            FTK_P();
            event.accept = true;
            p.mouse.pos = event.pos;

            switch (p.mouse.mode)
            {
            case Private::MouseMode::MoveNodes:
                if (p.mouse.move.has_value() && !p.mouse.autoScrollTimer)
                {
                    const ftk::V2I offset = event.pos - event.prev;
                    for (const auto i : p.mouse.moveNodes)
                    {
                        const auto j = p.nodeToPos.find(i);
                        if (j != p.nodeToPos.end())
                        {
                            j->second = j->second + offset;
                        }
                    }
                    setSizeUpdate();
                    setDrawUpdate();
                }
                break;

            case Private::MouseMode::ConnectNodes:
                if (p.mouse.connect.has_value())
                {
                    const auto& graph = p.document->getGraph();
                    std::shared_ptr<INodeGraphPort> port;
                    if (p.mouse.connect->input != -1)
                    {
                        const auto output = _getOutput(p.mouse.pos);
                        if (output.has_value())
                        {
                            port = output->widget->getOutputs()[output->output];
                        }
                    }
                    else if (p.mouse.connect->output != -1)
                    {
                        const auto input = _getInput(p.mouse.pos);
                        if (input.has_value())
                        {
                            port = input->widget->getInputs()[input->input];
                        }
                    }
                    if (port != p.mouse.connectPort)
                    {
                        if (p.mouse.connectPort)
                        {
                            p.mouse.connectPort->setBackgroundRole(ftk::ColorRole::None);
                        }
                        p.mouse.connectPort = port;
                        if (p.mouse.connectPort)
                        {
                            p.mouse.connectPort->setBackgroundRole(ftk::ColorRole::Checked);
                        }
                    }
                    setDrawUpdate();
                }
                break;

            case Private::MouseMode::Select:
                setDrawUpdate();
                break;

            case Private::MouseMode::Pan:
                if (auto scrollWidget = getParentT<ftk::ScrollWidget>())
                {
                    scrollWidget->setScrollPos(scrollWidget->getScrollPos() - (event.pos - event.prev));
                }
                break;

            default: break;
            }

            switch (p.mouse.mode)
            {
            case Private::MouseMode::MoveNodes:
            case Private::MouseMode::ConnectNodes:
            case Private::MouseMode::Select:
                if (auto scrollWidget = getParentT<ftk::ScrollWidget>())
                {
                    const ftk::Box2I scrollBox = ftk::margin(
                        scrollWidget->getScrollArea()->getGeometry(),
                        -p.size.margin);
                    if (!ftk::contains(scrollBox, p.mouse.pos))
                    {
                        if (!p.mouse.autoScrollTimer)
                        {
                            p.mouse.autoScrollTimer = ftk::Timer::create(getContext());
                            p.mouse.autoScrollTimer->setRepeating(true);
                            p.mouse.autoScrollTimer->start(
                                std::chrono::milliseconds(16),
                                [this]
                                {
                                    _autoScrollUpdate();
                                });
                        }
                    }
                    else
                    {
                        p.mouse.autoScrollTimer.reset();
                    }
                }
                break;
            default: break;
            }
        }

        void NodeGraphCanvas::mousePressEvent(ftk::MouseClickEvent& event)
        {
            FTK_P();
            event.accept = true;
            p.mouse.mode = Private::MouseMode::None;
            p.mouse.pressed = true;
            p.mouse.press = event.pos;
            p.mouse.canvasPress = event.pos - getGeometry().min;
            takeKeyFocus();

            // Connect nodes.
            if (Private::MouseMode::None == p.mouse.mode)
            {
                p.mouse.connect = _getConnect(event);
                if (p.mouse.connect)
                {
                    p.mouse.mode = Private::MouseMode::ConnectNodes;
                    moveToFront(p.mouse.connect->widget);
                }
            }

            // Move nodes.
            if (Private::MouseMode::None == p.mouse.mode)
            {
                p.mouse.move = _getMove(event);
                if (p.mouse.move)
                {
                    p.mouse.mode = Private::MouseMode::MoveNodes;
                    moveToFront(p.mouse.move->widget);
                    const auto& selection = p.document->getSelection();
                    const auto i = std::find(selection.begin(), selection.end(), p.mouse.move->node);
                    if (i == selection.end())
                    {
                        p.document->select({ p.mouse.move->node });
                    }
                }
            }

            // Selection.
            if (Private::MouseMode::None == p.mouse.mode)
            {
                if (ftk::MouseButton::Left == event.button)
                {
                    if (auto node = _getNode(p.mouse.pos))
                    {
                        p.mouse.mode = Private::MouseMode::Select;
                        if (ftk::checkKeyModifier(ftk::KeyModifier::Shift, event.modifiers))
                        {
                            p.document->selectionAdd({ node });
                        }
                        else if (ftk::checkKeyModifier(ftk::KeyModifier::Control, event.modifiers))
                        {
                            p.document->selectionRemove({ node });
                        }
                        else
                        {
                            p.document->select({ node });
                        }
                    }
                    else if (ftk::checkKeyModifier(ftk::KeyModifier::None, event.modifiers))
                    {
                        p.mouse.mode = Private::MouseMode::Select;
                    }
                    else if (ftk::checkKeyModifier(ftk::KeyModifier::Shift, event.modifiers))
                    {
                        p.mouse.mode = Private::MouseMode::Select;
                    }
                    else if (ftk::checkKeyModifier(ftk::KeyModifier::Control, event.modifiers))
                    {
                        p.mouse.mode = Private::MouseMode::Select;
                    }
                }
            }

            // Panning.
            if (Private::MouseMode::None == p.mouse.mode)
            {
                if (ftk::MouseButton::Middle == event.button &&
                    0 == event.modifiers)
                {
                    if (auto scrollWidget = getParentT<ftk::ScrollWidget>())
                    {
                        p.mouse.mode = Private::MouseMode::Pan;
                    }
                }
            }

            // Popup menu or clear the selection.
            if (Private::MouseMode::None == p.mouse.mode)
            {
                if ((ftk::MouseButton::Right == event.button && 0 == event.modifiers) ||
                    (ftk::MouseButton::Left == event.button && ftk::checkKeyModifier(ftk::KeyModifier::Alt, event.modifiers)))
                {
                    _popupMenu(event.pos);
                }
                else
                {
                    p.document->clearSelection();
                }
            }
        }

        void NodeGraphCanvas::mouseReleaseEvent(ftk::MouseClickEvent& event)
        {
            FTK_P();
            event.accept = true;
            p.mouse.pressed = false;
            p.mouse.autoScrollTimer.reset();

            switch (p.mouse.mode)
            {
            case Private::MouseMode::MoveNodes:
                if (p.mouse.move.has_value())
                {
                    std::vector<std::shared_ptr<render::INode> > nodes;
                    std::vector<ftk::V2I> pos;
                    for (const auto i : p.mouse.moveNodes)
                    {
                        const auto j = p.nodeToPos.find(i);
                        if (j != p.nodeToPos.end())
                        {
                            nodes.push_back(i);
                            pos.push_back(j->second);
                        }
                    }
                    const auto& graph = p.document->getGraph();
                    p.document->command(
                        render::MoveNodesCmd::create(graph, nodes, pos));
                    p.mouse.move.reset();
                }
                break;

            case Private::MouseMode::ConnectNodes:
                if (p.mouse.connect.has_value())
                {
                    const auto& graph = p.document->getGraph();
                    if (p.mouse.connect->input != -1)
                    {
                        const auto output = _getOutput(p.mouse.pos);
                        if (output.has_value())
                        {
                            p.document->command(
                                render::ConnectNodesCmd::create(
                                    graph,
                                    p.mouse.connect->node,
                                    p.mouse.connect->input,
                                    output->node,
                                    output->output));
                        }
                    }
                    else if (p.mouse.connect->output != -1)
                    {
                        const auto input = _getInput(p.mouse.pos);
                        if (input.has_value())
                        {
                            p.document->command(
                                render::ConnectNodesCmd::create(
                                    graph,
                                    input->node,
                                    input->input,
                                    p.mouse.connect->node,
                                    p.mouse.connect->output));
                        }
                    }
                    if (p.mouse.connectPort)
                    {
                        p.mouse.connectPort->setBackgroundRole(ftk::ColorRole::None);
                    }
                    p.mouse.connect.reset();
                    setDrawUpdate();
                }
                break;

            case Private::MouseMode::Select:
            {
                const auto nodes = _getNodes(_getSelectionRect());
                if (ftk::checkKeyModifier(ftk::KeyModifier::Shift, event.modifiers))
                {
                    p.document->selectionAdd(nodes);
                }
                else
                if (ftk::checkKeyModifier(ftk::KeyModifier::Control, event.modifiers))
                {
                    p.document->selectionRemove(nodes);
                }
                else
                {
                    p.document->select(nodes);
                }
                break;
            }

            default: break;
            }
            p.mouse.mode = Private::MouseMode::None;
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

        std::optional<NodeGraphCanvas::Move> NodeGraphCanvas::_getMove(const ftk::MouseClickEvent& event)
        {
            FTK_P();
            std::optional<NodeGraphCanvas::Move> out;
            if (ftk::MouseButton::Left == event.button &&
                0 == event.modifiers)
            {
                const auto& children = getChildren();
                for (auto i = children.rbegin(); i != children.rend(); ++i)
                {
                    if (auto widget = std::dynamic_pointer_cast<NodeGraphWidget>(*i))
                    {
                        const ftk::Box2I& g = widget->getGeometry();
                        if (ftk::contains(g, event.pos))
                        {
                            out = Move();
                            out->node = widget->getNode();
                            out->widget = widget;
                            break;
                        }
                    }
                }
            }
            return out;
        }

        std::optional<NodeGraphCanvas::Connect> NodeGraphCanvas::_getConnect(const ftk::MouseClickEvent& event)
        {
            std::optional<NodeGraphCanvas::Connect> out;
            if (ftk::MouseButton::Left == event.button &&
                0 == event.modifiers)
            {
                const auto& children = getChildren();
                for (auto i = children.rbegin(); i != children.rend(); ++i)
                {
                    if (auto widget = std::dynamic_pointer_cast<NodeGraphWidget>(*i))
                    {
                        const auto& inputs = widget->getInputs();
                        for (int i = 0; i < inputs.size(); ++i)
                        {
                            const ftk::Box2I& g = inputs[i]->getGeometry();
                            if (ftk::contains(g, event.pos))
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
                            if (ftk::contains(g, event.pos))
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

        ftk::Box2I NodeGraphCanvas::_getSelectionRect() const
        {
            FTK_P();
            ftk::Box2I out;
            const ftk::Box2I& g = getGeometry();
            out.min.x = std::min(g.min.x + p.mouse.canvasPress.x, p.mouse.pos.x);
            out.min.y = std::min(g.min.y + p.mouse.canvasPress.y, p.mouse.pos.y);
            out.max.x = std::max(g.min.x + p.mouse.canvasPress.x, p.mouse.pos.x);
            out.max.y = std::max(g.min.y + p.mouse.canvasPress.y, p.mouse.pos.y);
            return out;
        }

        std::shared_ptr<render::INode> NodeGraphCanvas::_getNode(const ftk::V2I& pos) const
        {
            FTK_P();
            std::shared_ptr<render::INode> out;
            const auto& children = getChildren();
            for (auto i = children.rbegin(); i != children.rend(); ++i)
            {
                if (ftk::contains((*i)->getGeometry(), pos))
                {
                    if (auto widget = std::dynamic_pointer_cast<NodeGraphWidget>(*i))
                    {
                        const auto j = p.widgetToNode.find(widget);
                        if (j != p.widgetToNode.end())
                        {
                            out = j->second;
                            break;
                        }
                    }
                }
            }
            return out;
        }

        std::vector<std::shared_ptr<render::INode> > NodeGraphCanvas::_getNodes(const ftk::Box2I& rect) const
        {
            FTK_P();
            std::vector<std::shared_ptr<render::INode> > out;
            for (const auto& i : p.nodeToWidget)
            {
                if (ftk::intersects(i.second->getGeometry(), rect))
                {
                    out.push_back(i.first);
                }
            }
            return out;
        }

        void NodeGraphCanvas::_popupMenu(const ftk::V2I& pos)
        {
            FTK_P();
            p.menu = ftk::Menu::create(getContext());

            auto editMenu = p.menu->addSubMenu("Edit");
            editMenu->addAction(p.editActions["Undo"]);
            editMenu->addAction(p.editActions["Redo"]);
            editMenu->addDivider();
            editMenu->addAction(p.editActions["SelectAll"]);
            editMenu->addAction(p.editActions["SelectNone"]);
            editMenu->addAction(p.editActions["SelectInvert"]);
            editMenu->addDivider();
            editMenu->addAction(p.editActions["Delete"]);

            auto nodeMenu = p.menu->addSubMenu("Node");
            for (const auto& group : p.nodeFactory->getGroups())
            {
                auto groupMenu = nodeMenu->addSubMenu(group);
                for (const auto& node : p.nodeFactory->getIDs(group))
                {
                    const auto info = p.nodeFactory->getInfo(node);
                    auto action = ftk::Action::create(
                        info.name,
                        [this, info, pos]
                        {
                            FTK_P();
                            if (auto node = p.nodeFactory->createNode(info.id))
                            {
                                const ftk::Box2I& g = getGeometry();
                                p.document->command(
                                    render::AddNodesCmd::create(
                                        p.document->getGraph(),
                                        { node },
                                        { pos - g.min }));
                            }
                        });
                    groupMenu->addAction(action);
                }
            }

            std::weak_ptr<NodeGraphCanvas> weak(std::dynamic_pointer_cast<NodeGraphCanvas>(shared_from_this()));
            p.menu->setCloseCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->menu.reset();
                    }
                });
            p.menu->open(
                getWindow(),
                ftk::Box2I(pos.x, pos.y, 0, 0));
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

            // Update widgets.
            for (const auto i : nodeToWidget)
            {
                i.second->setView(i.first == p.document->getViewNode());
                for (const auto& j : i.second->getOutputs())
                {
                    j->setConnect(false);
                }
            }
            for (const auto i : nodeToWidget)
            {
                const auto& inputs = i.first->getInputs();
                for (int j = 0; j < inputs.size(); ++j)
                {
                    const auto& input = inputs[j];
                    i.second->getInputs()[j]->setConnect(input.node.get());
                    if (input.node)
                    {
                        const auto k = nodeToWidget.find(input.node);
                        if (k != nodeToWidget.end())
                        {
                            k->second->getOutputs()[input.index]->setConnect(true);
                        }
                    }
                }
            }

            p.nodeToWidget = nodeToWidget;
            p.widgetToNode = widgetToNode;
            p.nodeToPos = nodeToPos;
            setSizeUpdate();
            setDrawUpdate();
        }

        void NodeGraphCanvas::_autoScrollUpdate()
        {
            FTK_P();
            if (auto scrollWidget = getParentT<ftk::ScrollWidget>())
            {
                const ftk::V2I scrollPosPrev = scrollWidget->getScrollPos();
                const ftk::V2F v = ftk::normalize(ftk::V2F(
                    p.mouse.pos.x - p.mouse.press.x,
                    p.mouse.pos.y - p.mouse.press.y)) * p.size.margin;
                scrollWidget->setScrollPos(scrollPosPrev + ftk::V2I(v.x, v.y));

                switch (p.mouse.mode)
                {
                case Private::MouseMode::MoveNodes:
                    if (p.mouse.move.has_value())
                    {
                        const ftk::V2I offset = scrollWidget->getScrollPos() - scrollPosPrev;
                        for (const auto i : p.mouse.moveNodes)
                        {
                            const auto j = p.nodeToPos.find(i);
                            if (j != p.nodeToPos.end())
                            {
                                j->second.x += offset.x;
                                j->second.y += offset.y;
                            }
                        }
                        setSizeUpdate();
                        setDrawUpdate();
                    }
                    break;
                case Private::MouseMode::ConnectNodes:
                case Private::MouseMode::Select:
                    setDrawUpdate();
                    break;
                default: break;
                }
            }
        }
    }
}
