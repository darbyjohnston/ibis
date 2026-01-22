// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "Viewport.h"

#include <ftk/UI/Label.h>

namespace ibis
{
    namespace ui
    {
        struct Viewport::Private
        {
            std::shared_ptr<ftk::Label> label;
        };

        void Viewport::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::Viewport", parent);
            FTK_P();
            p.label = ftk::Label::create(context, "Viewport", shared_from_this());
            p.label->setHAlign(ftk::HAlign::Center);
            p.label->setMarginRole(ftk::SizeRole::MarginLarge);
        }

        Viewport::Viewport() :
            _p(new Private)
        {}

        Viewport::~Viewport()
        {}

        std::shared_ptr<Viewport> Viewport::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<Viewport> out(new Viewport);
            out->_init(context, parent);
            return out;
        }

        ftk::Size2I Viewport::getSizeHint() const
        {
            return _p->label->getSizeHint();
        }

        void Viewport::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->label->setGeometry(value);
        }
    }
}
