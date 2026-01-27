// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#pragma once

#include <ibis/Models/TimeModel.h>

#include <ftk/UI/ToolBar.h>

namespace ibis
{
    namespace ui
    {
        //! Playback tool bar.
        class PlaybackToolBar : public ftk::ToolBar
        {
            FTK_NON_COPYABLE(PlaybackToolBar);

        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::TimeModel>&,
                const std::shared_ptr<IWidget>& parent);

            PlaybackToolBar();

        public:
            virtual ~PlaybackToolBar();

            //! Create a new widget.
            static std::shared_ptr<PlaybackToolBar> create(
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
