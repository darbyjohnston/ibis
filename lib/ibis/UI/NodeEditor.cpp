// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "NodeEditor.h"

#include <ftk/UI/Label.h>

namespace ibis
{
    namespace ui
    {
        struct NodeEditor::Private
        {
            std::shared_ptr<ftk::Label> label;
        };

        void NodeEditor::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::NodeEditor", parent);
            FTK_P();
            p.label = ftk::Label::create(context, "Node Editor", shared_from_this());
            p.label->setHAlign(ftk::HAlign::Center);
            p.label->setMarginRole(ftk::SizeRole::MarginLarge);
        }

        NodeEditor::NodeEditor() :
            _p(new Private)
        {}

        NodeEditor::~NodeEditor()
        {}

        std::shared_ptr<NodeEditor> NodeEditor::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<NodeEditor> out(new NodeEditor);
            out->_init(context, parent);
            return out;
        }

        ftk::Size2I NodeEditor::getSizeHint() const
        {
            return _p->label->getSizeHint();
        }

        void NodeEditor::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->label->setGeometry(value);
        }
    }
}
