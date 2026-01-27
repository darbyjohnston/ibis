// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include <ibis/UI/FrameToolBar.h>

#include <ftk/UI/Action.h>
#include <ftk/UI/ToolButton.h>

namespace ibis
{
    namespace ui
    {
        struct FrameToolBar::Private
        {
            std::map<std::string, std::shared_ptr<ftk::Action> > actions;
        };

        void FrameToolBar::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::TimeModel>& timeModel,
            const std::shared_ptr<IWidget>& parent)
        {
            ToolBar::_init(context, ftk::Orientation::Horizontal, parent);
            FTK_P();

            p.actions["Start"] = ftk::Action::create(
                "Start Frame",
                "FrameStart",
                ftk::Key::Home,
                [timeModel]
                {
                    timeModel->gotoStart();
                });
            p.actions["Start"]->setTooltip("Go to the start frame.");

            p.actions["Prev"] = ftk::Action::create(
                "Previous Frame",
                "FramePrev",
                ftk::Key::Left,
                [timeModel]
                {
                    timeModel->framePrev();
                });
            p.actions["Prev"]->setTooltip("Go to the previous frame.");

            p.actions["Next"] = ftk::Action::create(
                "Next Frame",
                "FrameNext",
                ftk::Key::Right,
                [timeModel]
                {
                    timeModel->frameNext();
                });
            p.actions["Next"]->setTooltip("Go to the next frame.");

            p.actions["End"] = ftk::Action::create(
                "End Frame",
                "FrameEnd",
                ftk::Key::End,
                [timeModel]
                {
                    timeModel->gotoEnd();
                });
            p.actions["End"]->setTooltip("Go to the end frame.");

            addAction(p.actions["Start"]);
            auto button = addAction(p.actions["Prev"]);
            button->setRepeatClick(true);
            button = addAction(p.actions["Next"]);
            button->setRepeatClick(true);
            addAction(p.actions["End"]);
        }

        FrameToolBar::FrameToolBar() :
            _p(new Private)
        {}

        FrameToolBar::~FrameToolBar()
        {}

        std::shared_ptr<FrameToolBar> FrameToolBar::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::TimeModel>& timeModel,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<FrameToolBar>(new FrameToolBar);
            out->_init(context, timeModel, parent);
            return out;
        }
            
        const std::map<std::string, std::shared_ptr<ftk::Action> >& FrameToolBar::getActions() const
        {
            return _p->actions;
        }
    }
}
