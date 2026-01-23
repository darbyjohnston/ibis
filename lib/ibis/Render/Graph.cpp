// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "Graph.h"

namespace ibis
{
    namespace render
    {
        struct Graph::Private
        {
            std::list<std::shared_ptr<INode> > nodes;
            std::map<std::shared_ptr<INode>, ftk::V2I> pos;
            std::shared_ptr<ftk::Observable<bool> > changed;
        };

        void Graph::_init(const std::shared_ptr<ftk::Context>& context)
        {
            FTK_P();
            p.changed = ftk::Observable<bool>::create(false);
        }

        Graph::Graph() :
            _p(new Private)
        {}

        Graph::~Graph()
        {}

        std::shared_ptr<Graph> Graph::create(const std::shared_ptr<ftk::Context>& context)
        {
            std::shared_ptr<Graph> out(new Graph);
            out->_init(context);
            return out;
        }

        void Graph::add(const std::shared_ptr<INode>& node, const ftk::V2I& pos)
        {
            FTK_P();
            p.nodes.push_back(node);
            p.pos[node] = pos;
            p.changed->setAlways(true);
        }

        void Graph::remove(const std::shared_ptr<INode>& node)
        {
            FTK_P();
            bool changed = false;
            const auto i = std::find(p.nodes.begin(), p.nodes.end(), node);
            if (i != p.nodes.end())
            {
                p.nodes.erase(i);
                changed = true;
            }
            const auto j = p.pos.find(node);
            if (j != p.pos.end())
            {
                p.pos.erase(j);
                changed = true;
            }
            if (changed)
            {
                p.changed->setAlways(true);
            }
        }

        const std::list<std::shared_ptr<INode> >& Graph::getNodes() const
        {
            return _p->nodes;
        }

        void Graph::move(const std::shared_ptr<INode>& node, const ftk::V2I& pos)
        {
            FTK_P();
            const auto i = p.pos.find(node);
            if (i != p.pos.end() && pos != i->second)
            {
                i->second = pos;
                p.changed->setAlways(true);
            }
        }

        ftk::V2I Graph::getPos(const std::shared_ptr<INode>& node)
        {
            FTK_P();
            ftk::V2I out;
            const auto i = p.pos.find(node);
            if (i != p.pos.end())
            {
                out = i->second;
            }
            return out;
        }

        void Graph::connect(
            const std::shared_ptr<INode>& inputNode,
            int input,
            const std::shared_ptr<INode>& outputNode,
            int output)
        {
            FTK_P();
            inputNode->setInput(input, NodeConnection(outputNode, output));
            p.changed->setAlways(true);
        }

        void Graph::disconnect(
            const std::shared_ptr<INode>& inputNode,
            int input,
            const std::shared_ptr<INode>& outputNode,
            int output)
        {
            FTK_P();
            inputNode->setInput(input, NodeConnection());
            p.changed->setAlways(true);
        }

        std::shared_ptr<ftk::IObservable<bool> > Graph::observe() const
        {
            return _p->changed;
        }
    }
}