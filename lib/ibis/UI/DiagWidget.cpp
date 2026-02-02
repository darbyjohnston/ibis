// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "DiagWidget.h"

#include <ftk/UI/GraphWidget.h>
#include <ftk/UI/RowLayout.h>
#include <ftk/UI/ScrollWidget.h>
#include <ftk/GL/Mesh.h>
#include <ftk/GL/OffscreenBuffer.h>
#include <ftk/GL/Shader.h>
#include <ftk/GL/Texture.h>
#include <ftk/Core/Format.h>
#include <ftk/Core/Timer.h>

namespace ibis
{
    namespace ui
    {
        struct DiagWidget::Private
        {
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

            p.graphs["GLObjects"] = ftk::GraphWidget::create(
                context,
                "OpenGL Objects",
                {
                    { ftk::ColorRole::Cyan, "Meshes: {0}" },
                    { ftk::ColorRole::Magenta, "Textures: {0}" },
                    { ftk::ColorRole::Yellow, "Buffers: {0}" },
                    { ftk::ColorRole::Red, "Shaders: {0}" }
                });

            p.graphs["GLMemory"] = ftk::GraphWidget::create(
                context,
                "OpenGL Memory",
                {
                    { ftk::ColorRole::Cyan, "Meshes: {0}MB" },
                    { ftk::ColorRole::Magenta, "Textures: {0}MB" },
                    { ftk::ColorRole::Yellow, "Buffers: {0}MB" }
                });

            p.graphs["Objects"] = ftk::GraphWidget::create(
                context,
                "Objects",
                {
                    { ftk::ColorRole::Cyan, "Images: {0}" },
                    { ftk::ColorRole::Magenta, "Widgets: {0}" }
                });

            p.graphs["Memory"] = ftk::GraphWidget::create(
                context,
                "Memory",
                {
                    { ftk::ColorRole::Cyan, "Images: {0}MB" }
                });

            auto layout = ftk::VerticalLayout::create(context);
            layout->setMarginRole(ftk::SizeRole::Margin);
            for (const auto& i : p.graphs)
            {
                i.second->setParent(layout);
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
            p.graphs["GLObjects"]->addSample(
                ftk::ColorRole::Cyan,
                ftk::gl::VBO::getObjectCount());
            p.graphs["GLObjects"]->addSample(
                ftk::ColorRole::Magenta,
                ftk::gl::Texture::getObjectCount());
            p.graphs["GLObjects"]->addSample(
                ftk::ColorRole::Yellow,
                ftk::gl::OffscreenBuffer::getObjectCount());
            p.graphs["GLObjects"]->addSample(
                ftk::ColorRole::Red,
                ftk::gl::Shader::getObjectCount());

            p.graphs["GLMemory"]->addSample(
                ftk::ColorRole::Cyan,
                ftk::gl::VBO::getTotalByteCount() / ftk::megabyte);
            p.graphs["GLMemory"]->addSample(
                ftk::ColorRole::Magenta,
                ftk::gl::Texture::getTotalByteCount() / ftk::megabyte);
            p.graphs["GLMemory"]->addSample(
                ftk::ColorRole::Yellow,
                ftk::gl::OffscreenBuffer::getTotalByteCount() / ftk::megabyte);

            p.graphs["Objects"]->addSample(
                ftk::ColorRole::Cyan,
                ftk::gl::VBO::getObjectCount());
            p.graphs["Objects"]->addSample(
                ftk::ColorRole::Magenta,
                ftk::gl::Texture::getObjectCount());

            p.graphs["Memory"]->addSample(
                ftk::ColorRole::Cyan,
                ftk::Image::getTotalByteCount() / ftk::megabyte);
        }
    }
}
