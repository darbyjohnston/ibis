// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "MessagesWidget.h"

#include <ibis/Models/MessagesModel.h>

#include <ftk/UI/Label.h>
#include <ftk/UI/RowLayout.h>
#include <ftk/UI/ScrollWidget.h>

namespace ibis
{
    namespace ui
    {
        struct MessagesWidget::Private
        {
            std::list<std::shared_ptr<ftk::Label> > labels;
            std::shared_ptr<ftk::VerticalLayout> layout;
            std::shared_ptr<ftk::ScrollWidget> scrollWidget;
            std::shared_ptr<ftk::ListObserver<std::string> > observer;
        };

        void MessagesWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::MessagesModel>& model,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::MessagesWidget", parent);
            FTK_P();

            p.layout = ftk::VerticalLayout::create(context);
            p.layout->setSpacingRole(ftk::SizeRole::None);

            p.scrollWidget = ftk::ScrollWidget::create(context, ftk::ScrollType::Both, shared_from_this());
            p.scrollWidget->setBorder(false);
            p.scrollWidget->setWidget(p.layout);

            p.observer = ftk::ListObserver<std::string>::create(
                model->observeMessages(),
                [this](const std::vector<std::string>& items)
                {
                    FTK_P();
                    p.layout->clear();
                    auto context = getContext();
                    for (const auto& item : items)
                    {
                        auto label = ftk::Label::create(context, item, p.layout);
                        label->setMarginRole(ftk::SizeRole::MarginInside);
                        p.labels.push_back(label);
                    }
                });
        }

        MessagesWidget::MessagesWidget() :
            _p(new Private)
        {}

        MessagesWidget::~MessagesWidget()
        {}

        std::shared_ptr<MessagesWidget> MessagesWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::MessagesModel>& model,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<MessagesWidget> out(new MessagesWidget);
            out->_init(context, model, parent);
            return out;
        }

        ftk::Size2I MessagesWidget::getSizeHint() const
        {
            return _p->scrollWidget->getSizeHint();
        }

        void MessagesWidget::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->scrollWidget->setGeometry(value);
        }
    }
}
