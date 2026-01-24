// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include "GraphCmdTest.h"

#include <ibis/Render/GraphCmd.h>

#include <ftk/Core/Assert.h>
#include <ftk/Core/Command.h>

namespace ibis
{
    namespace render_test
    {
        GraphCmdTest::GraphCmdTest(const std::shared_ptr<ftk::Context>& context) :
            ITest(context, "ibis::render_test::GraphCmdTest")
        {}

        GraphCmdTest::~GraphCmdTest()
        {}

        std::shared_ptr<GraphCmdTest> GraphCmdTest::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            return std::shared_ptr<GraphCmdTest>(new GraphCmdTest(context));
        }

        namespace
        {
            class TestNode : public render::INode
            {
            protected:
                void _init(const std::shared_ptr<ftk::Context>& context)
                {
                    INode::_init(context, "TestNode", 1);
                }

                TestNode() = default;

            public:
                virtual ~TestNode() = default;

                static std::shared_ptr<INode> create(
                    const std::shared_ptr<ftk::Context>& context)
                {
                    std::shared_ptr<TestNode> out(new TestNode);
                    out->_init(context);
                    return out;
                }
            };
        }

        void GraphCmdTest::run()
        {
            {
                auto cmdStack = ftk::CommandStack::create();
                auto graph = render::Graph::create(_context);
                auto node = TestNode::create(_context);
                ftk::V2I pos(100, 200);
                cmdStack->push(render::AddNodeCommand::create(graph, node, pos));
                FTK_ASSERT(!graph->getNodes().empty());
                FTK_ASSERT(node == graph->getNodes().front());
                FTK_ASSERT(pos == graph->getPos(node));
                
                cmdStack->undo();
                FTK_ASSERT(graph->getNodes().empty());
                cmdStack->redo();
                FTK_ASSERT(!graph->getNodes().empty());
                FTK_ASSERT(node == graph->getNodes().front());
                FTK_ASSERT(pos == graph->getPos(node));
            }
            {
                auto cmdStack = ftk::CommandStack::create();
                auto graph = render::Graph::create(_context);
                auto node = TestNode::create(_context);
                ftk::V2I pos(100, 200);
                cmdStack->push(render::AddNodeCommand::create(graph, node, pos));
 
                ftk::V2I pos2(300, 400);
                cmdStack->push(render::MoveNodeCommand::create(graph, node, pos2));
                FTK_ASSERT(pos2 == graph->getPos(node));
                cmdStack->undo();
                FTK_ASSERT(pos == graph->getPos(node));
                cmdStack->redo();
                FTK_ASSERT(pos2 == graph->getPos(node));
            }
            {
                auto cmdStack = ftk::CommandStack::create();
                auto graph = render::Graph::create(_context);
                auto node = TestNode::create(_context);
                cmdStack->push(render::AddNodeCommand::create(graph, node, ftk::V2I(100, 200)));
                auto node2 = TestNode::create(_context);
                cmdStack->push(render::AddNodeCommand::create(graph, node2, ftk::V2I(300, 400)));
                auto node3 = TestNode::create(_context);
                cmdStack->push(render::AddNodeCommand::create(graph, node3, ftk::V2I(500, 600)));

                cmdStack->push(render::ConnectNodesCommand::create(graph, node, 0, node2, 0));
                FTK_ASSERT(node->getInputs()[0].node == node2);
                FTK_ASSERT(node->getInputs()[0].index == 0);
                cmdStack->undo();
                FTK_ASSERT(!node->getInputs()[0].node);
                FTK_ASSERT(-1 == node->getInputs()[0].index);
                cmdStack->redo();
                FTK_ASSERT(node->getInputs()[0].node == node2);
                FTK_ASSERT(node->getInputs()[0].index == 0);

                cmdStack->push(render::ConnectNodesCommand::create(graph, node, 0, node3, 0));
                FTK_ASSERT(node->getInputs()[0].node == node3);
                FTK_ASSERT(node->getInputs()[0].index == 0);
                cmdStack->undo();
                FTK_ASSERT(node->getInputs()[0].node == node2);
                FTK_ASSERT(node->getInputs()[0].index == 0);
                cmdStack->redo();
                FTK_ASSERT(node->getInputs()[0].node == node3);
                FTK_ASSERT(node->getInputs()[0].index == 0);
            }
        }
    }
}
