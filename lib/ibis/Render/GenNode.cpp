// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "GenNode.h"

#include <ibis/Core/Time.h>

#include <ftk/UI/WidgetOptions.h>
#include <ftk/GL/GL.h>
#include <ftk/GL/OffscreenBuffer.h>
#include <ftk/Core/IRender.h>
#include <ftk/Core/Color.h>
#include <ftk/Core/Noise.h>

namespace ibis
{
    namespace render
    {
        struct SolidColorNode::Private
        {};

        void SolidColorNode::_init(const std::shared_ptr<ftk::Context>& context)
        {
            NodeAttr attr;
            attr["Size"] = ftk::Size2I(256, 256);
            attr["Color"] = ftk::Color4F(1.F, .7F, 0.F);
            INode::_init(context, getClassNodeInfo(), 0, 1, attr);
        }

        SolidColorNode::SolidColorNode() :
            _p(new Private)
        {}

        SolidColorNode::~SolidColorNode()
        {}

        NodeInfo SolidColorNode::getClassNodeInfo()
        {
            return { "SolidColor", "Solid Color", "Generator" };
        }

        std::shared_ptr<INode> SolidColorNode::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            std::shared_ptr<SolidColorNode> out(new SolidColorNode);
            out->_init(context);
            return out;
        }

        void SolidColorNode::exec(
            const std::shared_ptr<ftk::IRender>& render,
            const OTIO_NS::RationalTime& time)
        {
            INode::exec(render, time);
            FTK_P();
            const ftk::Size2I size = _attr->getItem("Size");
            if (size.isValid())
            {
                if (ftk::gl::doCreate(_outputs[0], size, ftk::ImageType::RGBA_F32))
                {
                    _outputs[0] = ftk::gl::OffscreenBuffer::create(size, ftk::ImageType::RGBA_F32);
                }
                ftk::gl::OffscreenBufferBinding binding(_outputs[0]);
                render->setRenderSize(size);
                render->setViewport(ftk::Box2I(0, 0, size.w, size.h));
                render->clearViewport(_attr->getItem("Color"));
            }

            _imageInfo->setItemOnlyIfChanged(
                0,
                _outputs[0] ?
                ftk::ImageInfo(_outputs[0]->getSize(), _outputs[0]->getType()) :
                ftk::ImageInfo());
        }

        struct GradientNode::Private
        {};

        void GradientNode::_init(const std::shared_ptr<ftk::Context>& context)
        {
            NodeAttr attr;
            attr["Size"] = ftk::Size2I(256, 256);
            attr["Color0"] = ftk::Color4F(0.F, 0.F, 0.F);
            attr["Color1"] = ftk::Color4F(1.F, 1.F, 1.F);
            attr["Orientation"] = ftk::Orientation::Vertical;
            INode::_init(context, getClassNodeInfo(), 0, 1, attr);
        }

        GradientNode::GradientNode() :
            _p(new Private)
        {}

        GradientNode::~GradientNode()
        {}

        NodeInfo GradientNode::getClassNodeInfo()
        {
            return { "Gradient", "Gradient", "Generator" };
        }

        std::shared_ptr<INode> GradientNode::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            std::shared_ptr<GradientNode> out(new GradientNode);
            out->_init(context);
            return out;
        }

        void GradientNode::exec(
            const std::shared_ptr<ftk::IRender>& render,
            const OTIO_NS::RationalTime& time)
        {
            INode::exec(render, time);
            FTK_P();
            const ftk::Size2I size = _attr->getItem("Size");
            if (size.isValid())
            {
                if (ftk::gl::doCreate(_outputs[0], size, ftk::ImageType::RGBA_F32))
                {
                    _outputs[0] = ftk::gl::OffscreenBuffer::create(size, ftk::ImageType::RGBA_F32);
                }
                ftk::gl::OffscreenBufferBinding binding(_outputs[0]);
                render->setRenderSize(size);
                render->setViewport(ftk::Box2I(0, 0, size.w, size.h));
                render->clearViewport(ftk::Color4F(0.F, 0.F, 0.F, 0.F));
                render->setTransform(_getProjection(size));

                ftk::TriMesh2F mesh;
                mesh.v.push_back(ftk::V2F(0.F, 0.F));
                mesh.v.push_back(ftk::V2F(size.w, 0.F));
                mesh.v.push_back(ftk::V2F(size.w, size.h));
                mesh.v.push_back(ftk::V2F(0.F, size.h));
                const ftk::Color4F color0 = _attr->getItem("Color0");
                const ftk::Color4F color1 = _attr->getItem("Color1");
                switch (static_cast<ftk::Orientation>(_attr->getItem("Orientation")))
                {
                case ftk::Orientation::Horizontal:
                    mesh.c.push_back(ftk::V4F(color0.r, color0.g, color0.b, color0.a));
                    mesh.c.push_back(ftk::V4F(color1.r, color1.g, color1.b, color1.a));
                    mesh.c.push_back(ftk::V4F(color1.r, color1.g, color1.b, color1.a));
                    mesh.c.push_back(ftk::V4F(color0.r, color0.g, color0.b, color0.a));
                    break;
                case ftk::Orientation::Vertical:
                    mesh.c.push_back(ftk::V4F(color1.r, color1.g, color1.b, color1.a));
                    mesh.c.push_back(ftk::V4F(color1.r, color1.g, color1.b, color1.a));
                    mesh.c.push_back(ftk::V4F(color0.r, color0.g, color0.b, color0.a));
                    mesh.c.push_back(ftk::V4F(color0.r, color0.g, color0.b, color0.a));
                    break;
                default: break;
                }
                mesh.triangles.push_back(
                    {
                        ftk::Vertex2(1, 0, 1),
                        ftk::Vertex2(3, 0, 3),
                        ftk::Vertex2(2, 0, 2)
                    });
                mesh.triangles.push_back(
                    {
                        ftk::Vertex2(1, 0, 1),
                        ftk::Vertex2(4, 0, 4),
                        ftk::Vertex2(3, 0, 3)
                    });
                render->drawColorMesh(mesh);
            }

            _imageInfo->setItemOnlyIfChanged(
                0,
                _outputs[0] ?
                ftk::ImageInfo(_outputs[0]->getSize(), _outputs[0]->getType()) :
                ftk::ImageInfo());
        }

        struct NoiseNode::Private
        {
            ftk::Noise noise;
            float scale = 0.F;
            ftk::Size2I size;
            std::shared_ptr<ftk::Image> image;
        };

        void NoiseNode::_init(const std::shared_ptr<ftk::Context>& context)
        {
            NodeAttr attr;
            attr["Size"] = ftk::Size2I(256, 256);
            attr["Scale"] = 1.F;
            INode::_init(context, getClassNodeInfo(), 0, 1, attr);
        }

        NoiseNode::NoiseNode() :
            _p(new Private)
        {}

        NoiseNode::~NoiseNode()
        {}

        NodeInfo NoiseNode::getClassNodeInfo()
        {
            return { "Noise", "Noise", "Generator" };
        }

        std::shared_ptr<INode> NoiseNode::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            std::shared_ptr<NoiseNode> out(new NoiseNode);
            out->_init(context);
            return out;
        }

        void NoiseNode::exec(
            const std::shared_ptr<ftk::IRender>& render,
            const OTIO_NS::RationalTime& time)
        {
            INode::exec(render, time);
            FTK_P();
            const ftk::Size2I size = _attr->getItem("Size");
            if (size.isValid())
            {
                const float scale = _attr->getItem("Scale");
                if (scale != p.scale || size != p.size)
                {
                    p.scale = scale;
                    p.size = size;
                    p.image = ftk::Image::create(size, ftk::ImageType::L_U8);
                    for (int y = 0; y < size.h; ++y)
                    {
                        uint8_t* dataP = p.image->getData() + size.w * y;
                        const float v = y / 100.F;
                        for (int x = 0; x < size.w; ++x)
                        {
                            const float u = x / 100.F;
                            const float w = 0.F;
                            *dataP++ = (p.noise.get(
                                u * p.scale,
                                v * p.scale,
                                w * p.scale) + 1.0) / 2.0 * 255;
                        }
                    }
                }

                if (ftk::gl::doCreate(_outputs[0], size, ftk::ImageType::RGBA_F32))
                {
                    _outputs[0] = ftk::gl::OffscreenBuffer::create(size, ftk::ImageType::RGBA_F32);
                }
                ftk::gl::OffscreenBufferBinding binding(_outputs[0]);
                render->setRenderSize(size);
                render->setViewport(ftk::Box2I(0, 0, size.w, size.h));
                render->clearViewport(ftk::Color4F(0.F, 0.F, 0.F, 0.F));
                render->setTransform(_getProjection(size));
                render->drawImage(p.image, ftk::Box2I(0, 0, size.w, size.h));
            }
            else
            {
                p.image.reset();
            }

            _imageInfo->setItemOnlyIfChanged(
                0,
                _outputs[0] ?
                ftk::ImageInfo(_outputs[0]->getSize(), _outputs[0]->getType()) :
                ftk::ImageInfo());
        }
    }
}
