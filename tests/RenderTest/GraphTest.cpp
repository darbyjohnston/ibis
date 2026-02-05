// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

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
                void _init(
                    const std::shared_ptr<ftk::Context>& context,
                    const nlohmann::json& json)
                {
                    INode::_init(context, getNodeInfo(), 1, 1, {}, json);
                }

                TestNode() = default;

            public:
                virtual ~TestNode() = default;

                static render::NodeInfo getNodeInfo() { return { "TestNode", "Test Node", "Test" }; }

                static std::shared_ptr<INode> create(
                    const std::shared_ptr<ftk::Context>& context,
                    const nlohmann::json& json = {})
                {
                    std::shared_ptr<TestNode> out(new TestNode);
                    out->_init(context, json);
                    return out;
                }
            };
        }

        void GraphTest::run()
        {
            {
                auto graph = render::Graph::create(_context);
                FTK_ASSERT(graph->getNodes().empty());
                FTK_ASSERT(graph->getLeafNodes().empty());

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
                FTK_ASSERT(1 == graph->getNodes().size());
                FTK_ASSERT(node == graph->getNodes()[0]);
                FTK_ASSERT(pos == graph->getPos(node));
                FTK_ASSERT(1 == graph->getLeafNodes().size());
                FTK_ASSERT(node == graph->getLeafNodes()[0]);
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
                FTK_ASSERT(1 == graph->getLeafNodes().size());
                FTK_ASSERT(node == graph->getLeafNodes()[0]);
                FTK_ASSERT(changed);

                changed = false;
                graph->disconnect(node, 0);
                FTK_ASSERT(!node->getInputs()[0].node);
                FTK_ASSERT(-1 == node->getInputs()[0].index);
                FTK_ASSERT(2 == graph->getLeafNodes().size());
                FTK_ASSERT(changed);

                changed = false;
                graph->remove({ node, node2 });
                FTK_ASSERT(graph->getNodes().empty());
                FTK_ASSERT(changed);

                changed = false;
                graph->add({ node, node2 });
                FTK_ASSERT(2 == graph->getNodes().size());
                FTK_ASSERT(node == graph->getNodes()[0]);
                FTK_ASSERT(node2 == graph->getNodes()[1]);
                FTK_ASSERT(changed);

                changed = false;
                pos = ftk::V2I(100, 200);
                ftk::V2I pos2(300, 400);
                graph->move({ node, node2 }, { pos, pos2 });
                FTK_ASSERT(std::vector<ftk::V2I>({ pos, pos2 }) == graph->getPos({node, node2}));
                FTK_ASSERT(changed);
                changed = false;
                graph->move({ node, node2 }, { pos, pos2 });
                FTK_ASSERT(!changed);
                pos = ftk::V2I(10, 20);
                pos2 = ftk::V2I(30, 40);
                changed = false;
                graph->move({ node, node2 }, { pos, pos2 });
                FTK_ASSERT(changed);
            }
            {
                auto graph = render::Graph::create(_context);
                auto node = TestNode::create(_context);
                auto node2 = TestNode::create(_context);
                auto node3 = TestNode::create(_context);
                auto node4 = TestNode::create(_context);
                std::vector<ftk::V2I> pos =
                {
                    ftk::V2I(100, 100),
                    ftk::V2I(100, 200),
                    ftk::V2I(100, 300),
                    ftk::V2I(100, 400)
                };
                graph->add({ node, node2, node3, node4 }, pos);
                graph->connect(node3, 0, node2, 0);
                graph->connect(node4, 0, node2, 0);
                graph->connect(node2, 0, node, 0);
                auto json = graph->to_json();
                _print(json.dump(4));

                auto nodeFactory = render::NodeFactory::create(_context);
                nodeFactory->add(TestNode::getNodeInfo().id, &TestNode::create);
                auto graph2 = render::Graph::create(_context, json, nodeFactory);
                FTK_ASSERT(graph->getNodes().size() == graph2->getNodes().size());
                node = graph2->getNodes()[0];
                node2 = graph2->getNodes()[1];
                node3 = graph2->getNodes()[2];
                node4 = graph2->getNodes()[3];
                FTK_ASSERT(pos[0] == graph2->getPos(node));
                FTK_ASSERT(pos[1] == graph2->getPos(node2));
                FTK_ASSERT(pos[2] == graph2->getPos(node3));
                FTK_ASSERT(pos[3] == graph2->getPos(node4));
                FTK_ASSERT(node2->getInputs()[0].node == node);
                FTK_ASSERT(node3->getInputs()[0].node == node2);
                FTK_ASSERT(node4->getInputs()[0].node == node2);
            }
        }
    }
}
