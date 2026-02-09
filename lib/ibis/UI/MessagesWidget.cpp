// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "MessagesWidget.h"

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
        struct MessagesWidget::Private
        {
            std::weak_ptr<ftk::Settings> settings;
            bool autoScroll = true;

            std::shared_ptr<ftk::TextEdit> textEdit;
            std::shared_ptr<ftk::PushButton> copyButton;
            std::shared_ptr<ftk::CheckBox> autoScrollCheckBox;
            std::shared_ptr<ftk::VerticalLayout> layout;

            std::shared_ptr<ftk::ListObserver<std::string> > observer;
        };

        void MessagesWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::SettingsModel>& settings,
            const std::shared_ptr<models::MessagesModel>& model,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::MessagesWidget", parent);
            FTK_P();

            p.settings = settings;
            settings->get("/MessagesWidget/AutoScroll", p.autoScroll);

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
                model->observeMessages(),
                [this](const std::vector<std::string>& items)
                {
                    _p->textEdit->setText(items);
                });

            p.copyButton->setClickedCallback(
                [this, model]
                {
                    auto clipboard = getContext()->getSystem<ftk::ClipboardSystem>();
                    clipboard->setText(ftk::join(model->observeMessages()->get(), '\n'));
                });

            p.autoScrollCheckBox->setCheckedCallback(
                [this](bool value)
                {
                    _p->autoScroll = value;
                });
        }

        MessagesWidget::MessagesWidget() :
            _p(new Private)
        {}

        MessagesWidget::~MessagesWidget()
        {
            FTK_P();
            if (auto settings = p.settings.lock())
            {
                settings->set("/MessagesWidget/AutoScroll", p.autoScroll);
            }
        }

        std::shared_ptr<MessagesWidget> MessagesWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::SettingsModel>& settings,
            const std::shared_ptr<models::MessagesModel>& model,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<MessagesWidget> out(new MessagesWidget);
            out->_init(context, settings, model, parent);
            return out;
        }

        ftk::Size2I MessagesWidget::getSizeHint() const
        {
            return _p->layout->getSizeHint();
        }

        void MessagesWidget::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }
    }
}
