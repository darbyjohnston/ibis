// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "DocumentEditor.h"

#include <ibis/Models/DocumentModel.h>

#include <ftk/UI/CheckBox.h>
#include <ftk/UI/FormLayout.h>
#include <ftk/UI/IntEdit.h>
#include <ftk/UI/RowLayout.h>
#include <ftk/UI/ScrollWidget.h>

namespace ibis
{
    namespace ui
    {
        struct DocumentEditor::Private
        {
            std::shared_ptr<models::Document> document;

            std::shared_ptr<ftk::IntEdit> canvasWidthEdit;
            std::shared_ptr<ftk::IntEdit> canvasHeightEdit;
            std::shared_ptr<ftk::CheckBox> gridCheckBox;
            std::shared_ptr<ftk::IntEdit> gridSizeEdit;
            std::shared_ptr<ftk::ScrollWidget> scrollWidget;

            std::shared_ptr<ftk::Observer<std::shared_ptr<models::Document> > > documentObserver;
            std::shared_ptr<ftk::Observer<ftk::Size2I> > canvasSizeObserver;
            std::shared_ptr<ftk::Observer<bool> > gridObserver;
            std::shared_ptr<ftk::Observer<int> > gridSizeObserver;
        };

        void DocumentEditor::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::DocumentModel>& model,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::DocumentEditor", parent);
            FTK_P();

            p.canvasWidthEdit = ftk::IntEdit::create(context);
            p.canvasWidthEdit->setRange(1000, 8000);
            p.canvasWidthEdit->setStep(100);
            p.canvasWidthEdit->setLargeStep(1000);

            p.canvasHeightEdit = ftk::IntEdit::create(context);
            p.canvasHeightEdit->setRange(1000, 8000);
            p.canvasHeightEdit->setStep(100);
            p.canvasHeightEdit->setLargeStep(1000);

            p.gridCheckBox = ftk::CheckBox::create(context);
            p.gridCheckBox->setHStretch(ftk::Stretch::Expanding);

            p.gridSizeEdit = ftk::IntEdit::create(context);
            p.gridSizeEdit->setRange(10, 100);
            p.gridSizeEdit->setStep(10);
            p.gridSizeEdit->setLargeStep(10);

            auto layout = ftk::FormLayout::create(context, shared_from_this());
            layout->setMarginRole(ftk::SizeRole::Margin);
            layout->setSpacingRole(ftk::SizeRole::SpacingSmall);
            auto hLayout = ftk::HorizontalLayout::create(context);
            hLayout->setSpacingRole(ftk::SizeRole::SpacingTool);
            p.canvasWidthEdit->setParent(hLayout);
            p.canvasHeightEdit->setParent(hLayout);
            layout->addRow("Canvas size:", hLayout);
            layout->addRow("Grid:", p.gridCheckBox);
            layout->addRow("Grid size:", p.gridSizeEdit);
            p.scrollWidget = ftk::ScrollWidget::create(context, ftk::ScrollType::Both, shared_from_this());
            p.scrollWidget->setBorder(false);
            p.scrollWidget->setWidget(layout);

            p.documentObserver = ftk::Observer<std::shared_ptr<models::Document> >::create(
                model->observeCurrent(),
                [this](const std::shared_ptr<models::Document>& document)
                {
                    FTK_P();
                    p.document = document;
                    if (document)
                    {
                        p.canvasSizeObserver = ftk::Observer<ftk::Size2I>::create(
                            document->observeCanvasSize(),
                            [this](const ftk::Size2I& value)
                            {
                                FTK_P();
                                p.canvasWidthEdit->setValue(value.w);
                                p.canvasHeightEdit->setValue(value.h);
                            });
                        p.gridObserver = ftk::Observer<bool>::create(
                            document->observeGrid(),
                            [this](bool value)
                            {
                                FTK_P();
                                p.gridCheckBox->setChecked(value);
                            });
                        p.gridSizeObserver = ftk::Observer<int>::create(
                            document->observeGridSize(),
                            [this](int value)
                            {
                                FTK_P();
                                p.gridSizeEdit->setValue(value);
                            });
                    }
                    else
                    {
                        p.canvasSizeObserver.reset();
                        p.gridObserver.reset();
                        p.gridSizeObserver.reset();
                    }
                    p.canvasWidthEdit->setEnabled(document.get());
                    p.canvasHeightEdit->setEnabled(document.get());
                    p.gridCheckBox->setEnabled(document.get());
                    p.gridSizeEdit->setEnabled(document.get());
                });

            p.canvasWidthEdit->setCallback(
                [this](int value)
                {
                    FTK_P();
                    if (p.document)
                    {
                        ftk::Size2I size = p.document->getCanvasSize();
                        size.w = value;
                        p.document->setCanvasSize(size);
                    }
                });

            p.canvasHeightEdit->setCallback(
                [this](int value)
                {
                    FTK_P();
                    if (p.document)
                    {
                        ftk::Size2I size = p.document->getCanvasSize();
                        size.h = value;
                        p.document->setCanvasSize(size);
                    }
                });

            p.gridCheckBox->setCheckedCallback(
                [this](bool value)
                {
                    FTK_P();
                    if (p.document)
                    {
                        p.document->setGrid(value);
                    }
                });

            p.gridSizeEdit->setCallback(
                [this](int value)
                {
                    FTK_P();
                    if (p.document)
                    {
                        p.document->setGridSize(value);
                    }
                });
        }

        DocumentEditor::DocumentEditor() :
            _p(new Private)
        {}

        DocumentEditor::~DocumentEditor()
        {}

        std::shared_ptr<DocumentEditor> DocumentEditor::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<models::DocumentModel>& model,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            std::shared_ptr<DocumentEditor> out(new DocumentEditor);
            out->_init(context, model, parent);
            return out;
        }

        ftk::Size2I DocumentEditor::getSizeHint() const
        {
            return _p->scrollWidget->getSizeHint();
        }

        void DocumentEditor::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->scrollWidget->setGeometry(value);
        }
    }
}
