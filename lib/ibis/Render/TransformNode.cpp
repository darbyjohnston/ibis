// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "TransformNode.h"

#include <ftk/GL/GL.h>
#include <ftk/GL/Mesh.h>
#include <ftk/GL/OffscreenBuffer.h>
#include <ftk/GL/Shader.h>
#include <ftk/Core/IRender.h>

namespace ibis
{
    namespace render
    {
        struct ResizeNode::Private
        {};

        void ResizeNode::_init(const std::shared_ptr<ftk::Context>& context)
        {
            NodeAttr attr;
            attr["Width"] = 1920;
            attr["Height"] = 1080;
            INode::_init(context, getClassNodeInfo(), 1, 1, attr);
        }

        ResizeNode::ResizeNode() :
            _p(new Private)
        {}

        ResizeNode::~ResizeNode()
        {}

        NodeInfo ResizeNode::getClassNodeInfo()
        {
            return { "Resize", "Resize", "Transform" };
        }

        std::shared_ptr<INode> ResizeNode::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            std::shared_ptr<ResizeNode> out(new ResizeNode);
            out->_init(context);
            return out;
        }

        void ResizeNode::exec(
            const std::shared_ptr<ftk::IRender>& render,
            const OTIO_NS::RationalTime& time)
        {
            INode::exec(render, time);
            FTK_P();

            ftk::gl::TextureInfo info;
            if (_inputs->getItem(0).node)
            {
                const auto& input0 = _inputs->getItem(0).node->getOutputs();
                if (!input0.empty() && input0.front())
                {
                    info.size.w = _attr->getItem("Width");
                    info.size.h = _attr->getItem("Height");
                    info.type = input0.front()->getType();
                }
                if (info.isValid())
                {
                    if (ftk::gl::doCreate(_outputs[0], info))
                    {
                        _outputs[0] = ftk::gl::OffscreenBuffer::create(info);
                    }
                    ftk::gl::OffscreenBufferBinding binding(_outputs[0]);
                    render->setRenderSize(info.size);
                    const ftk::Box2I vp(0, 0, info.size.w, info.size.h);
                    render->setViewport(vp);
                    render->clearViewport(ftk::Color4F(0.F, 0.F, 0.F, 0.F));
                    render->setTransform(_getProjection(info.size));
                    render->drawTexture(input0.front()->getColorID(), vp, true);
                }
            }
            if (!_inputs->getItem(0).node || !info.isValid())
            {
                _outputs[0].reset();
            }
            _textureInfo->setItemOnlyIfChanged(0, info);
        }

        struct CropNode::Private
        {};

        void CropNode::_init(const std::shared_ptr<ftk::Context>& context)
        {
            NodeAttr attr;
            attr["X"] = 200;
            attr["Y"] = 200;
            attr["Width"] = 400;
            attr["Height"] = 400;
            INode::_init(context, getClassNodeInfo(), 1, 1, attr);
        }

        CropNode::CropNode() :
            _p(new Private)
        {}

        CropNode::~CropNode()
        {}

        NodeInfo CropNode::getClassNodeInfo()
        {
            return { "Crop", "Crop", "Transform" };
        }

        std::shared_ptr<INode> CropNode::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            std::shared_ptr<CropNode> out(new CropNode);
            out->_init(context);
            return out;
        }

        void CropNode::exec(
            const std::shared_ptr<ftk::IRender>& render,
            const OTIO_NS::RationalTime& time)
        {
            INode::exec(render, time);
            FTK_P();

            ftk::gl::TextureInfo info;
            if (_inputs->getItem(0).node)
            {
                const auto& input0 = _inputs->getItem(0).node->getOutputs();
                ftk::Size2I inputSize;
                if (!input0.empty() && input0.front())
                {
                    info.size.w = _attr->getItem("Width");
                    info.size.h = _attr->getItem("Height");
                    info.type = input0.front()->getType();
                    inputSize = input0.front()->getSize();
                }
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

                    const int x = _attr->getItem("X");
                    const int y = _attr->getItem("Y");
                    const ftk::Box2I rect(
                        -ftk::V2I(x, inputSize.h - 1 - y - info.size.h),
                        inputSize);
                    render->drawTexture(input0.front()->getColorID(), rect, true);
                }
            }
            if (!_inputs->getItem(0).node || !info.isValid())
            {
                _outputs[0].reset();
            }
            _textureInfo->setItemOnlyIfChanged(0, info);
        }

        struct MirrorNode::Private
        {
            std::shared_ptr<ftk::gl::Shader> shader;
        };

        void MirrorNode::_init(const std::shared_ptr<ftk::Context>& context)
        {
            NodeAttr attr;
            attr["Horizontal"] = true;
            attr["Vertical"] = false;
            INode::_init(context, getClassNodeInfo(), 1, 1, attr);
        }

        MirrorNode::MirrorNode() :
            _p(new Private)
        {}

        MirrorNode::~MirrorNode()
        {}

        NodeInfo MirrorNode::getClassNodeInfo()
        {
            return { "Mirror", "Mirror", "Transform" };
        }

        std::shared_ptr<INode> MirrorNode::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            std::shared_ptr<MirrorNode> out(new MirrorNode);
            out->_init(context);
            return out;
        }

        namespace
        {
            const std::string mirrorVertex =
                "#version 410\n"
                "\n"
                "in vec3 vPos;\n"
                "in vec2 vTexture;\n"
                "out vec2 fTexture;\n"
                "\n"
                "struct Transform\n"
                "{\n"
                "    mat4 mvp;\n"
                "};\n"
                "\n"
                "uniform Transform transform;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    gl_Position = transform.mvp * vec4(vPos, 1.0);\n"
                "    fTexture = vTexture;\n"
                "}\n";

            const std::string mirrorFragment =
                "#version 410\n"
                "\n"
                "in vec2 fTexture;\n"
                "out vec4 outColor;\n"
                "\n"
                "uniform sampler2D textureSampler;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    outColor = texture(textureSampler, fTexture);\n"
                "}\n";

            ftk::TriMesh2F mesh(const ftk::Box2I& box, bool mirrorH, bool mirrorV)
            {
                ftk::TriMesh2F out;

                const auto& min = box.min;
                const auto& max = box.max;
                out.v.push_back(ftk::V2F(min.x, min.y));
                out.v.push_back(ftk::V2F(max.x + 1, min.y));
                out.v.push_back(ftk::V2F(max.x + 1, max.y + 1));
                out.v.push_back(ftk::V2F(min.x, max.y + 1));
                out.t.push_back(ftk::V2F(mirrorH ? 1.F : 0.F, mirrorV ? 1.F : 0.F));
                out.t.push_back(ftk::V2F(mirrorH ? 0.F : 1.F, mirrorV ? 1.F : 0.F));
                out.t.push_back(ftk::V2F(mirrorH ? 0.F : 1.F, mirrorV ? 0.F : 1.F));
                out.t.push_back(ftk::V2F(mirrorH ? 1.F : 0.F, mirrorV ? 0.F : 1.F));

                ftk::Triangle2 triangle;
                triangle.v[0].v = 1;
                triangle.v[1].v = 3;
                triangle.v[2].v = 2;
                triangle.v[0].t = 1;
                triangle.v[1].t = 3;
                triangle.v[2].t = 2;
                out.triangles.push_back(triangle);
                triangle.v[0].v = 3;
                triangle.v[1].v = 1;
                triangle.v[2].v = 4;
                triangle.v[0].t = 3;
                triangle.v[1].t = 1;
                triangle.v[2].t = 4;
                out.triangles.push_back(triangle);

                return out;
            }
        }

        void MirrorNode::exec(
            const std::shared_ptr<ftk::IRender>& render,
            const OTIO_NS::RationalTime& time)
        {
            INode::exec(render, time);
            FTK_P();

            if (!p.shader)
            {
                p.shader = ftk::gl::Shader::create(mirrorVertex, mirrorFragment);
            }

            ftk::gl::TextureInfo info;
            if (_inputs->getItem(0).node)
            {
                const auto& input0 = _inputs->getItem(0).node->getOutputs();
                if (!input0.empty() && input0.front())
                {
                    info = input0.front()->getInfo();
                }
                if (info.isValid())
                {
                    if (ftk::gl::doCreate(_outputs[0], info))
                    {
                        _outputs[0] = ftk::gl::OffscreenBuffer::create(info);
                    }
                    ftk::gl::OffscreenBufferBinding binding(_outputs[0]);
                    render->setRenderSize(info.size);
                    const ftk::Box2I rect(0, 0, info.size.w, info.size.h);
                    render->setViewport(rect);
                    render->clearViewport(ftk::Color4F(0.F, 0.F, 0.F, 0.F));
                    p.shader->bind();
                    p.shader->setUniform("transform.mvp", _getProjection(info.size));
                    p.shader->setUniform("textureSampler", 0);

                    glActiveTexture(static_cast<GLenum>(GL_TEXTURE0));
                    glBindTexture(GL_TEXTURE_2D, input0.front()->getColorID());

                    auto vbo = ftk::gl::VBO::create(2 * 3, ftk::gl::VBOType::Pos2_F32_UV_U16);
                    const bool horizontal = _attr->getItem("Horizontal");
                    const bool vertical = _attr->getItem("Vertical");
                    vbo->copy(
                        convert(mesh(ftk::Box2I(0, 0, info.size.w, info.size.h), horizontal, !vertical),
                        vbo->getType()));
                    auto vao = ftk::gl::VAO::create(vbo->getType(), vbo->getID());
                    vao->bind();
                    vao->draw(GL_TRIANGLES, 0, vbo->getSize());
                }
            }
            if (!_inputs->getItem(0).node || !info.isValid())
            {
                _outputs[0].reset();
            }
            _textureInfo->setItemOnlyIfChanged(0, info);
        }

        struct RotateNode::Private
        {};

        void RotateNode::_init(const std::shared_ptr<ftk::Context>& context)
        {
            NodeAttr attr;
            attr["Rotate"] = 90.F;
            INode::_init(context, getClassNodeInfo(), 1, 1, attr);
        }

        RotateNode::RotateNode() :
            _p(new Private)
        {}

        RotateNode::~RotateNode()
        {}

        NodeInfo RotateNode::getClassNodeInfo()
        {
            return { "Rotate", "Rotate", "Transform" };
        }

        std::shared_ptr<INode> RotateNode::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            std::shared_ptr<RotateNode> out(new RotateNode);
            out->_init(context);
            return out;
        }

        void RotateNode::exec(
            const std::shared_ptr<ftk::IRender>& render,
            const OTIO_NS::RationalTime& time)
        {
            INode::exec(render, time);
            FTK_P();

            ftk::gl::TextureInfo info;
            if (_inputs->getItem(0).node)
            {
                const float rotate = _attr->getItem("Rotate");
                const ftk::M44F m = ftk::rotateZ(rotate);
                const auto& input0 = _inputs->getItem(0).node->getOutputs();
                ftk::Size2I input0Size;
                if (!input0.empty() && input0.front())
                {
                    input0Size = input0.front()->getSize();
                    std::vector<ftk::V3F> pts;
                    pts.push_back(ftk::V3F(0.F, 0.F, 0.F));
                    pts.push_back(ftk::V3F(input0Size.w, 0.F, 0.F));
                    pts.push_back(ftk::V3F(input0Size.w, input0Size.h, 0.F));
                    pts.push_back(ftk::V3F(0.F, input0Size.h, 0.F));
                    std::vector<ftk::V3F> pts2;
                    for (const auto& pt : pts)
                    {
                        pts2.push_back(m * pt);
                    }
                    const ftk::Box3F bbox = ftk::bbox(pts2);
                    info.size.w = bbox.w();
                    info.size.h = bbox.h();
                    info.type = input0.front()->getType();
                }
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
                    const ftk::M44F m2 =
                        ftk::translate(ftk::V3F(info.size.w / 2, info.size.h / 2, 0.F)) *
                        m *
                        ftk::translate(ftk::V3F(-input0Size.w / 2, -input0Size.h / 2, 0.F));
                    render->setTransform(_getProjection(info.size) * m2);
                    render->drawTexture(input0.front()->getColorID(), ftk::Box2I(0, 0, input0Size.w, input0Size.h), true);
                }
            }
            if (!_inputs->getItem(0).node || !info.isValid())
            {
                _outputs[0].reset();
            }
            _textureInfo->setItemOnlyIfChanged(0, info);
        }
    }
}
