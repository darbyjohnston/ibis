// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "SysLogWidget.h"

#include <ibis/Models/MessagesModel.h>
#include <ibis/Models/SettingsModel.h>

#include <ftk/UI/CheckBox.h>
#include <ftk/UI/ClipboardSystem.h>
#include <ftk/UI/PushButton.h>
#include <ftk/UI/RowLayout.h>
#include <ftk/UI/TextEdit.h>
#include <ftk/Core/String.h>

namespace ibis
{
    namespace ui
    {
        struct SysLogWidget::Private
        {
            std::weak_ptr<ftk::Settings> settings;
            bool autoScroll = true;

            std::shared_ptr<ftk::TextEdit> textEdit;
            std::shared_ptr<ftk::PushButton> copyButton;
            std::shared_ptr<ftk::CheckBox> autoScrollCheckBox;
            std::shared_ptr<ftk::VerticalLayout> layout;

            std::shared_ptr<ftk::ListObserver<std::string> > observer;
        };

        void SysLogWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::SettingsModel>& settings,
            const std::shared_ptr<models::MessagesModel>& model,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::SysLogWidget", parent);
            FTK_P();

            p.settings = settings;
            settings->get("/SysLogWidget/AutoScroll", p.autoScroll);

            p.textEdit = ftk::TextEdit::create(context);
            p.textEdit->setReadOnly(true);
            p.textEdit->setVStretch(ftk::Stretch::Expanding);

            p.copyButton = ftk::PushButton::create(context, "Copy");

            p.autoScrollCheckBox = ftk::CheckBox::create(context, "Auto-scroll");
            p.autoScrollCheckBox->setChecked(p.autoScroll);

            p.layout = ftk::VerticalLayout::create(context, shared_from_this());
            p.layout->setMarginRole(ftk::SizeRole::MarginSmall);
            p.layout->setSpacingRole(ftk::SizeRole::SpacingSmall);
            p.textEdit->setParent(p.layout);
            auto hLayout = ftk::HorizontalLayout::create(context, p.layout);
            p.copyButton->setParent(hLayout);
            hLayout->addSpacer(ftk::Stretch::Expanding);
            p.autoScrollCheckBox->setParent(hLayout);

            p.observer = ftk::ListObserver<std::string>::create(
                model->observeLog(),
                [this](const std::vector<std::string>& items)
                {
                    FTK_P();
                    p.textEdit->setText(items);
                    if (p.autoScroll)
                    {
                        p.textEdit->getModel()->setCursor(ftk::TextEditPos(items.size(), 0));
                    }
                });

            p.copyButton->setClickedCallback(
                [this, model]
                {
                    auto clipboard = getContext()->getSystem<ftk::ClipboardSystem>();
                    clipboard->setText(ftk::join(model->observeLog()->get(), '\n'));
                });

            p.autoScrollCheckBox->setCheckedCallback(
                [this](bool value)
                {
                    _p->autoScroll = value;
                });
        }

        SysLogWidget::SysLogWidget() :
            _p(new Private)
        {}

        SysLogWidget::~SysLogWidget()
        {
            FTK_P();
            if (auto settings = p.settings.lock())
            {
                settings->set("/SysLogWidget/AutoScroll", p.autoScroll);
            }
        }

        std::shared_ptr<SysLogWidget> SysLogWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::SettingsModel>& settings,
            const std::shared_ptr<models::MessagesModel>& model,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<SysLogWidget> out(new SysLogWidget);
            out->_init(context, settings, model, parent);
            return out;
        }

        ftk::Size2I SysLogWidget::getSizeHint() const
        {
            return _p->layout->getSizeHint();
        }

        void SysLogWidget::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }
    }
}
