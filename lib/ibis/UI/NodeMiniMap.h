// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#pragma once

#include <ftk/UI/ScrollWidget.h>

namespace ibis
{
    namespace ui
    {
        //! Node mini-map widget.
        class NodeMiniMap : public ftk::IWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ftk::IWidget>& parent);

            NodeMiniMap();

        public:
            virtual ~NodeMiniMap();

            //! Create a new widget.
            static std::shared_ptr<NodeMiniMap> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);

            //! Set the scroll information.
            void setScrollInfo(const ftk::ScrollInfo&);

            //! Set the callback.
            void setCallback(const std::function<void(const ftk::V2I&)>&);

            //! Set the child geometry.
            void setChildGeometry(const std::vector<ftk::Box2I>&);

            //! Get the child geometry color role.
            ftk::ColorRole getChildColorRole() const;

            //! Set the child geometry color role.
            void setChildColorRole(ftk::ColorRole);

            void setGeometry(const ftk::Box2I&) override;
            void sizeHintEvent(const ftk::SizeHintEvent&) override;
            void drawEvent(const ftk::Box2I&, const ftk::DrawEvent&) override;

        private:
            FTK_PRIVATE();
        };
    }
}
