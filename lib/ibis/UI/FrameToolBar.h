// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#pragma once

#include <ibis/Models/TimeModel.h>

#include <ftk/UI/ToolBar.h>

namespace ibis
{
    namespace ui
    {
        //! Frame tool bar.
        class FrameToolBar : public ftk::ToolBar
        {
            FTK_NON_COPYABLE(FrameToolBar);

        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::TimeModel>&,
                const std::shared_ptr<IWidget>& parent);

            FrameToolBar();

        public:
            virtual ~FrameToolBar();

            //! Create a new widget.
            static std::shared_ptr<FrameToolBar> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::TimeModel>&,
                const std::shared_ptr<IWidget>& parent = nullptr);
            
            //! Get the actions.
            const std::map<std::string, std::shared_ptr<ftk::Action> >& getActions() const;

        private:
            FTK_PRIVATE();
        };
    }
}
