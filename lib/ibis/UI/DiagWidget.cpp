// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "DiagWidget.h"

#include <ftk/UI/DiagWidget.h>
#include <ftk/UI/ScrollWidget.h>

namespace ibis
{
    namespace ui
    {
        struct DiagWidget::Private
        {
            std::shared_ptr<ftk::DiagWidget> widget;
            std::shared_ptr<ftk::ScrollWidget> scrollWidget;
        };

        void DiagWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ftk::DiagModel>& model,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::DiagWidget", parent);
            FTK_P();

            p.widget = ftk::DiagWidget::create(context, model);
            p.widget->setMarginRole(ftk::SizeRole::Margin);

            p.scrollWidget = ftk::ScrollWidget::create(context, ftk::ScrollType::Both, shared_from_this());
            p.scrollWidget->setBorder(false);
            p.scrollWidget->setWidget(p.widget);
        }

        DiagWidget::DiagWidget() :
            _p(new Private)
        {}

        DiagWidget::~DiagWidget()
        {}

        std::shared_ptr<DiagWidget> DiagWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ftk::DiagModel>& model,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<DiagWidget> out(new DiagWidget);
            out->_init(context, model, parent);
            return out;
        }

        ftk::Size2I DiagWidget::getSizeHint() const
        {
            return _p->scrollWidget->getSizeHint();
        }

        void DiagWidget::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->scrollWidget->setGeometry(value);
        }
    }
}
