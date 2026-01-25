// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "NodeEditor.h"

#include "NodeWidgetFactory.h"

#include <ibis/Models/Document.h>
#include <ibis/Models/NodeSelectionModel.h>

#include <ftk/UI/RowLayout.h>
#include <ftk/UI/ScrollWidget.h>

namespace ibis
{
    namespace ui
    {
        struct NodeEditor::Private
        {
            std::shared_ptr<ftk::VerticalLayout> layout;
            std::shared_ptr<ftk::ScrollWidget> scrollWidget;

            std::shared_ptr<ftk::ListObserver<std::shared_ptr<render::INode> > > selectionObserver;
        };

        void NodeEditor::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<NodeWidgetFactory>& factory,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::NodeEditor", parent);
            FTK_P();

            p.layout = ftk::VerticalLayout::create(context);

            p.scrollWidget = ftk::ScrollWidget::create(context, ftk::ScrollType::Both, shared_from_this());
            p.scrollWidget->setBorder(false);
            p.scrollWidget->setWidget(p.layout);

            p.selectionObserver = ftk::ListObserver<std::shared_ptr<render::INode> >::create(
                document->observeSelection(),
                [this, factory, document](const std::vector<std::shared_ptr<render::INode> >& selection)
                {
                    FTK_P();
                    p.layout->clear();
                    for (const auto& node : selection)
                    {
                        auto widget = factory->createWidget(document->getGraph(), node);
                        widget->setParent(p.layout);
                    }
                });
        }

        NodeEditor::NodeEditor() :
            _p(new Private)
        {}

        NodeEditor::~NodeEditor()
        {}

        std::shared_ptr<NodeEditor> NodeEditor::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<NodeWidgetFactory>& factory,
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<NodeEditor> out(new NodeEditor);
            out->_init(context, factory, document, parent);
            return out;
        }

        ftk::Size2I NodeEditor::getSizeHint() const
        {
            return _p->scrollWidget->getSizeHint();
        }

        void NodeEditor::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->scrollWidget->setGeometry(value);
        }
    }
}
