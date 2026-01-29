// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "DiagWidget.h"

#include <ftk/UI/RowLayout.h>

namespace ibis
{
    namespace ui
    {
        struct DiagWidget::Private
        {
            std::shared_ptr<ftk::VerticalLayout> layout;
        };

        void DiagWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::DiagWidget", parent);
            FTK_P();

            p.layout = ftk::VerticalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(ftk::SizeRole::None);
        }

        DiagWidget::DiagWidget() :
            _p(new Private)
        {}

        DiagWidget::~DiagWidget()
        {}

        std::shared_ptr<DiagWidget> DiagWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<DiagWidget> out(new DiagWidget);
            out->_init(context, parent);
            return out;
        }

        ftk::Size2I DiagWidget::getSizeHint() const
        {
            return _p->layout->getSizeHint();
        }

        void DiagWidget::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }
    }
}
