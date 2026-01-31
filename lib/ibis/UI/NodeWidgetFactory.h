// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ibis/UI/INodeWidget.h>

namespace ibis
{
    namespace ui
    {
        //! Node widget factory.
        class NodeWidgetFactory : public std::enable_shared_from_this<NodeWidgetFactory>
        {
        protected:
            void _init(const std::shared_ptr<ftk::Context>&);

            NodeWidgetFactory();

        public:
            virtual ~NodeWidgetFactory();

            //! Create a new factory.
            static std::shared_ptr<NodeWidgetFactory> create(
                const std::shared_ptr<ftk::Context>&);
            
            //! Get node IDs.
            std::vector<std::string> getNodeIDs() const;

            //! Create a new widget.
            std::shared_ptr<INodeWidget> createWidget(
                const std::shared_ptr<models::Document>&,
                const std::shared_ptr<render::INode>&);

        private:
            FTK_PRIVATE();
        };
    }
}
