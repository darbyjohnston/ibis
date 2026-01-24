// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "GraphRender.h"

namespace ibis
{
    namespace render
    {
        struct GraphRender::Private
        {
        };

        void GraphRender::_init(
            const std::shared_ptr<ftk::Context>& context)
        {
            FTK_P();
        }

        GraphRender::GraphRender()
        {}

        GraphRender::~GraphRender()
        {}

        std::shared_ptr<GraphRender> GraphRender::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            std::shared_ptr<GraphRender> out(new GraphRender);
            out->_init(context);
            return out;
        }

        void GraphRender::render(
            const std::shared_ptr<Graph>& graph,
            const GraphRenderOptions& options)
        {

        }
    }
}
