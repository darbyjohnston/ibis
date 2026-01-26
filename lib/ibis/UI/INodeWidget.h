// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ftk/UI/IWidget.h>

namespace ibis
{
    namespace render
    {
        class INode;
    }

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

            //! Get the ID.
            const std::string& getID() const;

        protected:
            std::shared_ptr<ibis::models::Document> _document;
            std::shared_ptr<ibis::render::INode> _node;
        };
    }
}
