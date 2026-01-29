// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "SettingsWidget.h"

#include <ftk/UI/RowLayout.h>

namespace ibis
{
    namespace ui
    {
        struct SettingsWidget::Private
        {
            std::shared_ptr<ftk::VerticalLayout> layout;
        };

        void SettingsWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::SettingsWidget", parent);
            FTK_P();

            p.layout = ftk::VerticalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(ftk::SizeRole::None);
        }

        SettingsWidget::SettingsWidget() :
            _p(new Private)
        {}

        SettingsWidget::~SettingsWidget()
        {}

        std::shared_ptr<SettingsWidget> SettingsWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<SettingsWidget> out(new SettingsWidget);
            out->_init(context, app, parent);
            return out;
        }

        ftk::Size2I SettingsWidget::getSizeHint() const
        {
            return _p->layout->getSizeHint();
        }

        void SettingsWidget::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }
    }
}
