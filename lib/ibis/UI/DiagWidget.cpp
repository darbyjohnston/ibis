// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "DiagWidget.h"

#include <ftk/UI/Divider.h>
#include <ftk/UI/GraphWidget.h>
#include <ftk/UI/Label.h>
#include <ftk/UI/RowLayout.h>
#include <ftk/UI/ScrollWidget.h>
#include <ftk/gl/Mesh.h>
#include <ftk/gl/OffscreenBuffer.h>
#include <ftk/gl/Shader.h>
#include <ftk/gl/Texture.h>
#include <ftk/Core/Format.h>
#include <ftk/Core/Timer.h>

namespace ibis
{
    namespace ui
    {
        struct DiagWidget::Private
        {
            std::map<std::string, std::shared_ptr<ftk::Label> > labels;
            std::map<std::string, std::shared_ptr<ftk::GraphWidget> > graphs;
            std::shared_ptr<ftk::Timer> timer;
            std::shared_ptr<ftk::ScrollWidget> scrollWidget;
        };

        void DiagWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::DiagWidget", parent);
            FTK_P();

            const std::vector<std::string> graphs =
            {
                "Images",
                "ImagesSize",
                "Meshes",
                "MeshesSize",
                "Textures",
                "TexturesSize",
                "OffscreenBuffers",
                "Shaders",
                "Widgets"
            };
            for (const auto& graph : graphs)
            {
                p.labels[graph] = ftk::Label::create(context);
                p.graphs[graph] = ftk::GraphWidget::create(context);
            }

            auto layout = ftk::VerticalLayout::create(context);
            layout->setSpacingRole(ftk::SizeRole::None);
            for (const auto& graph : graphs)
            {
                auto vLayout = ftk::VerticalLayout::create(context, layout);
                vLayout->setMarginRole(ftk::SizeRole::Margin);
                vLayout->setSpacingRole(ftk::SizeRole::SpacingSmall);
                p.labels[graph]->setParent(vLayout);
                p.graphs[graph]->setParent(vLayout);
                ftk::Divider::create(context, ftk::Orientation::Vertical, layout);
            }

            p.scrollWidget = ftk::ScrollWidget::create(context, ftk::ScrollType::Both, shared_from_this());
            p.scrollWidget->setWidget(layout);
            p.scrollWidget->setBorder(false);

            _widgetUpdate();

            p.timer = ftk::Timer::create(context);
            p.timer->setRepeating(true);
            p.timer->start(
                std::chrono::milliseconds(1000),
                [this]
                {
                    _widgetUpdate();
                });
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
            return _p->scrollWidget->getSizeHint();
        }

        void DiagWidget::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->scrollWidget->setGeometry(value);
        }

        void DiagWidget::_widgetUpdate()
        {
            FTK_P();

            size_t count = ftk::Image::getObjectCount();
            p.labels["Images"]->setText(ftk::Format("Image objects: {0}").arg(count));
            p.graphs["Images"]->addSample(count);

            count = ftk::Image::getTotalByteCount() / ftk::megabyte;
            p.labels["ImagesSize"]->setText(ftk::Format("Total images size: {0}MB").arg(count));
            p.graphs["ImagesSize"]->addSample(count);

            count = ftk::gl::VBO::getObjectCount();
            p.labels["Meshes"]->setText(ftk::Format("Mesh objects: {0}").arg(count));
            p.graphs["Meshes"]->addSample(count);

            count = ftk::gl::VBO::getTotalByteCount() / ftk::megabyte;
            p.labels["MeshesSize"]->setText(ftk::Format("Total meshes size: {0}MB").arg(count));
            p.graphs["MeshesSize"]->addSample(count);

            count = ftk::gl::Texture::getObjectCount();
            p.labels["Textures"]->setText(ftk::Format("Texture objects: {0}").arg(count));
            p.graphs["Textures"]->addSample(count);

            count = ftk::gl::Texture::getTotalByteCount() / ftk::megabyte;
            p.labels["TexturesSize"]->setText(ftk::Format("Total textures size: {0}MB").arg(count));
            p.graphs["TexturesSize"]->addSample(count);

            count = ftk::gl::OffscreenBuffer::getObjectCount();
            p.labels["OffscreenBuffers"]->setText(ftk::Format("Offscreen buffers: {0}").arg(count));
            p.graphs["OffscreenBuffers"]->addSample(count);

            count = ftk::gl::Shader::getObjectCount();
            p.labels["Shaders"]->setText(ftk::Format("Shaders: {0}").arg(count));
            p.graphs["Shaders"]->addSample(count);

            count = ftk::IWidget::getObjectCount();
            p.labels["Widgets"]->setText(ftk::Format("Widgets: {0}").arg(count));
            p.graphs["Widgets"]->addSample(count);
        }
    }
}
