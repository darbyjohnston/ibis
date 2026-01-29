// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "SysLogWidget.h"

#include <ftk/UI/RowLayout.h>

namespace ibis
{
    namespace ui
    {
        struct SysLogWidget::Private
        {
            std::shared_ptr<ftk::VerticalLayout> layout;
        };

        void SysLogWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::SysLogWidget", parent);
            FTK_P();

            p.layout = ftk::VerticalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(ftk::SizeRole::None);
        }

        SysLogWidget::SysLogWidget() :
            _p(new Private)
        {}

        SysLogWidget::~SysLogWidget()
        {}

        std::shared_ptr<SysLogWidget> SysLogWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<SysLogWidget> out(new SysLogWidget);
            out->_init(context, parent);
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
