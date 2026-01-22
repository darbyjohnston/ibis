// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ftk/UI/IWidget.h>

namespace ibis
{
    namespace ui
    {
        //! Viewport.
        class Viewport : public ftk::IWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ftk::IWidget>& parent);

            Viewport();

        public:
            virtual ~Viewport();

            //! Create a new widget.
            static std::shared_ptr<Viewport> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;

        private:
            FTK_PRIVATE();
        };
    }
}
