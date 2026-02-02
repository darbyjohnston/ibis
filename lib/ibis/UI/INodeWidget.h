// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ibis/Render/INode.h>

#include <ftk/UI/IWidget.h>

namespace ibis
{
    namespace models
    {
        class Document;
    }

    namespace ui
    {
        //! Base class for node widgets.
        class INodeWidget : public ftk::IWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ibis::models::Document>&,
                const std::shared_ptr<ibis::render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent);

            INodeWidget() = default;

        public:
            virtual ~INodeWidget() = 0;

            //! Get the node information.
            const render::NodeInfo& getNodeInfo() const;

            //! Get the node.
            const std::shared_ptr<ibis::render::INode>& getNode() const;

        protected:
            std::shared_ptr<ibis::models::Document> _document;
            std::shared_ptr<ibis::render::INode> _node;
        };

        //! Base class for node widgets with extended interactions.
        class IInteractionNodeWidget : public INodeWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ibis::models::Document>&,
                const std::shared_ptr<ibis::render::INode>&,
                const std::shared_ptr<ftk::IWidget>& parent);

            IInteractionNodeWidget();

        public:
            virtual ~IInteractionNodeWidget() = 0;

        protected:
            void _callback(
                const std::vector<std::pair<std::string, nlohmann::json> >&,
                bool pressed);

        private:
            FTK_PRIVATE();
        };
    }
}
