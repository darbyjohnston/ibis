// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include <ibis/UI/StatusBar.h>

#include <ftk/UI/Label.h>
#include <ftk/UI/SysLogModel.h>
#include <ftk/Core/Timer.h>

namespace ibis
{
    namespace ui
    {
        struct StatusBar::Private
        {
            std::shared_ptr<ftk::Label> label;
            std::function<void(void)> callback;
            std::shared_ptr<ftk::Timer> timer;
            std::shared_ptr<ftk::ListObserver<std::string> > observer;
        };

        void StatusBar::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ftk::SysLogModel>& model,
            const std::shared_ptr<IWidget>& parent)
        {
            IMouseWidget::_init(context, "ibis::ui::StatusBar", parent);
            FTK_P();

            setTooltip(
                "The status bar displays the last message or error.\n"
                "\n"
                "Click to show additional messages and errors.");
            _setMouseHoverEnabled(true);
            _setMousePressEnabled(true);

            p.label = ftk::Label::create(context, shared_from_this());
            p.label->setMarginRole(ftk::SizeRole::MarginInside);

            p.timer = ftk::Timer::create(context);

            p.observer = ftk::ListObserver<std::string>::create(
                model->observeMessages(),
                [this](const std::vector<std::string>& items)
                {
                    FTK_P();
                    p.label->setText(!items.empty() ? items.back() : std::string());
                    p.label->setTooltip(!items.empty() ? items.back() : std::string());
                    if (!items.empty())
                    {
                        p.timer->start(
                            std::chrono::seconds(5),
                            [this]
                            {
                                _p->label->setText(std::string());
                                _p->label->setTooltip(std::string());
                            });
                    }
                });
        }

        StatusBar::StatusBar() :
            _p(new Private)
        {}

        StatusBar::~StatusBar()
        {}

        std::shared_ptr<StatusBar> StatusBar::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ftk::SysLogModel>& model,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<StatusBar>(new StatusBar);
            out->_init(context, model, parent);
            return out;
        }

        void StatusBar::setCallback(const std::function<void(void)>& value)
        {
            _p->callback = value;
        }

        ftk::Size2I StatusBar::getSizeHint() const
        {
            return _p->label->getSizeHint();
        }

        void StatusBar::setGeometry(const ftk::Box2I& value)
        {
            IMouseWidget::setGeometry(value);
            _p->label->setGeometry(value);
        }

        void StatusBar::mousePressEvent(ftk::MouseClickEvent& event)
        {
            IMouseWidget::mousePressEvent(event);
            FTK_P();
            if (p.callback)
            {
                p.callback();
            }
        }
    }
}
