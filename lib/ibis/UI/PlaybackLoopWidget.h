// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#pragma once

#include <ibis/Models/TimeModel.h>

#include <ftk/UI/IWidget.h>

namespace ibis
{
    namespace ui
    {
        //! Playback loop widget.
        class PlaybackLoopWidget : public ftk::IWidget
        {
            FTK_NON_COPYABLE(PlaybackLoopWidget);

        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<IWidget>& parent);

            PlaybackLoopWidget();

        public:
            virtual ~PlaybackLoopWidget();

            //! Create a new widget.
            static std::shared_ptr<PlaybackLoopWidget> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            //! Get the playback loop.
            models::PlaybackLoop getLoop() const;

            //! Set the playback loop.
            void setLoop(models::PlaybackLoop);

            //! Set the callback.
            void setCallback(const std::function<void(models::PlaybackLoop)>&);

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;

        private:
            void _widgetUpdate();
            void _showPopup();

            FTK_PRIVATE();
        };
    }
}
