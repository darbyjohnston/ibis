// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include "GraphTest.h"

#include <ibis/Render/Graph.h>
#include <ibis/Render/INode.h>
#include <ibis/Render/NodeFactory.h>

#include <ftk/Core/Assert.h>

namespace ibis
{
    namespace render_test
    {
        GraphTest::GraphTest(const std::shared_ptr<ftk::Context>& context) :
            ITest(context, "ibis::render_test::GraphTest")
        {}

        GraphTest::~GraphTest()
        {}

        std::shared_ptr<GraphTest> GraphTest::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            return std::shared_ptr<GraphTest>(new GraphTest(context));
        }

        namespace
        {
            class TestNode : public render::INode
            {
            protected:
                void _init(const std::shared_ptr<ftk::Context>& context)
                {
                    INode::_init(context, getNodeID(), 1);
                }

                TestNode() = default;

            public:
                virtual ~TestNode() = default;

                static std::string getNodeID() { return "TestNode"; }

                static std::shared_ptr<INode> create(
                    const std::shared_ptr<ftk::Context>& context)
                {
                    std::shared_ptr<TestNode> out(new TestNode);
                    out->_init(context);
                    return out;
                }
            };
        }

        void GraphTest::run()
        {
            {
                auto graph = render::Graph::create(_context);
                FTK_ASSERT(graph->getNodes().empty());

                bool changed = false;
                auto changedObserver = ftk::Observer<bool>::create(
                    graph->observe(),
                    [&changed](bool value)
                    {
                        changed = value;
                    });

                auto node = TestNode::create(_context);
                ftk::V2I pos(100, 200);
                graph->add(node, pos);
                FTK_ASSERT(!graph->getNodes().empty());
                FTK_ASSERT(node == graph->getNodes().front());
                FTK_ASSERT(pos == graph->getPos(node));
                FTK_ASSERT(changed);

                changed = false;
                graph->remove(node);
                FTK_ASSERT(graph->getNodes().empty());
                FTK_ASSERT(changed);

                graph->add(node, pos);
                pos = ftk::V2I(10, 20);
                changed = false;
                graph->move(node, pos);
                FTK_ASSERT(pos == graph->getPos(node));
                FTK_ASSERT(changed);
                changed = false;

                auto node2 = TestNode::create(_context);
                pos = ftk::V2I(30, 40);
                graph->add(node2, pos);
                changed = false;
                graph->connect(node, 0, node2, 0);
                FTK_ASSERT(node->getInputs()[0].node == node2);
                FTK_ASSERT(node->getInputs()[0].index == 0);
                FTK_ASSERT(changed);

                changed = false;
                graph->disconnect(node, 0, node2, 0);
                FTK_ASSERT(!node->getInputs()[0].node);
                FTK_ASSERT(-1 == node->getInputs()[0].index);
                FTK_ASSERT(changed);
            }
            {
                auto graph = render::Graph::create(_context);
                auto node = TestNode::create(_context);
                ftk::V2I pos(100, 200);
                graph->add(node, pos);
                auto json = graph->to_json();
                _print(json.dump(4));

                auto nodeFactory = render::NodeFactory::create(_context);
                nodeFactory->add(TestNode::getNodeID(), &TestNode::create);
                auto graph2 = render::Graph::create(_context, json, nodeFactory);
                FTK_ASSERT(graph->getNodes().size() == graph2->getNodes().size());
            }
        }
    }
}
