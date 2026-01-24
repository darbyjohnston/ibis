// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ftk/Core/Context.h>
#include <ftk/Core/Size.h>

namespace ibis
{
    namespace render
    {
        class Graph;
        class INode;

        //! Graph renderer options.
        struct GraphRenderOptions
        {
            std::shared_ptr<INode> node;
            int                    output = 0;
            ftk::Size2I            size;
        };

        //! Graph renderer.
        class GraphRender : public std::enable_shared_from_this<GraphRender>
        {
        protected:
            void _init(const std::shared_ptr<ftk::Context>&);

            GraphRender();

        public:
            virtual ~GraphRender();

            //! Create a new renderer.
            static std::shared_ptr<GraphRender> create(
                const std::shared_ptr<ftk::Context>&);

            //! Render the graph.
            void render(
                const std::shared_ptr<Graph>&,
                const GraphRenderOptions&);

        private:
            FTK_PRIVATE();
        };
    }
}
