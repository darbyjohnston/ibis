// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "GenNode.h"

#include <ibis/Core/Time.h>

#include <ftk/UI/WidgetOptions.h>
#include <ftk/GL/GL.h>
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
            attr["Type"] = ftk::gl::TextureType::RGBA_F32;
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

            const ftk::gl::TextureInfo info(_attr->getItem("Size"), _attr->getItem("Type"));
            if (info.isValid())
            {
                if (ftk::gl::doCreate(_outputs[0], info))
                {
                    _outputs[0] = ftk::gl::OffscreenBuffer::create(info);
                }
                ftk::gl::OffscreenBufferBinding binding(_outputs[0]);
                render->setRenderSize(info.size);
                render->setViewport(ftk::Box2I(0, 0, info.size.w, info.size.h));
                render->clearViewport(_attr->getItem("Color"));
            }
            _textureInfo->setItemOnlyIfChanged(0, info);
        }

        struct GradientNode::Private
        {};

        void GradientNode::_init(const std::shared_ptr<ftk::Context>& context)
        {
            NodeAttr attr;
            attr["Size"] = ftk::Size2I(256, 256);
            attr["Type"] = ftk::gl::TextureType::RGBA_F32;
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

            const ftk::gl::TextureInfo info(_attr->getItem("Size"), _attr->getItem("Type"));
            if (info.isValid())
            {
                if (ftk::gl::doCreate(_outputs[0], info))
                {
                    _outputs[0] = ftk::gl::OffscreenBuffer::create(info);
                }
                ftk::gl::OffscreenBufferBinding binding(_outputs[0]);
                render->setRenderSize(info.size);
                render->setViewport(ftk::Box2I(0, 0, info.size.w, info.size.h));
                render->clearViewport(ftk::Color4F(0.F, 0.F, 0.F, 0.F));
                render->setTransform(_getProjection(info.size));

                ftk::TriMesh2F mesh;
                mesh.v.push_back(ftk::V2F(0.F, 0.F));
                mesh.v.push_back(ftk::V2F(info.size.w, 0.F));
                mesh.v.push_back(ftk::V2F(info.size.w, info.size.h));
                mesh.v.push_back(ftk::V2F(0.F, info.size.h));
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
            _textureInfo->setItemOnlyIfChanged(0, info);
        }

        struct NoiseNode::Private
        {
            ftk::Noise noise;
            float scale = 0.F;
            ftk::gl::TextureInfo info;
            std::shared_ptr<ftk::Image> image;
        };

        void NoiseNode::_init(const std::shared_ptr<ftk::Context>& context)
        {
            NodeAttr attr;
            attr["Size"] = ftk::Size2I(256, 256);
            attr["Type"] = ftk::gl::TextureType::RGBA_F32;
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

            const ftk::gl::TextureInfo info(_attr->getItem("Size"), _attr->getItem("Type"));
            if (info.isValid())
            {
                const float scale = _attr->getItem("Scale");
                if (scale != p.scale || info != p.info)
                {
                    p.scale = scale;
                    p.info = info;
                    p.image = ftk::Image::create(info.size, ftk::ImageType::L_U8);
                    for (int y = 0; y < info.size.h; ++y)
                    {
                        uint8_t* dataP = p.image->getData() + info.size.w * y;
                        const float v = y / 100.F;
                        for (int x = 0; x < info.size.w; ++x)
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

                if (ftk::gl::doCreate(_outputs[0], info))
                {
                    _outputs[0] = ftk::gl::OffscreenBuffer::create(info);
                }
                ftk::gl::OffscreenBufferBinding binding(_outputs[0]);
                render->setRenderSize(info.size);
                render->setViewport(ftk::Box2I(0, 0, info.size.w, info.size.h));
                render->clearViewport(ftk::Color4F(0.F, 0.F, 0.F, 0.F));
                render->setTransform(_getProjection(info.size));
                render->drawImage(p.image, ftk::Box2I(0, 0, info.size.w, info.size.h));
            }
            else
            {
                p.image.reset();
            }
            _textureInfo->setItemOnlyIfChanged(0, info);
        }
    }
}
