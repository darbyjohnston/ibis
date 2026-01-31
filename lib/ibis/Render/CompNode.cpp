// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "CompNode.h"

#include <ftk/GL/GL.h>
#include <ftk/GL/Mesh.h>
#include <ftk/GL/OffscreenBuffer.h>
#include <ftk/GL/Shader.h>
#include <ftk/Core/Error.h>
#include <ftk/Core/Format.h>
#include <ftk/Core/IRender.h>
#include <ftk/Core/String.h>

namespace ibis
{
    namespace render
    {
        FTK_ENUM_IMPL(
            OverMode,
            "Premult",
            "NonPremult",
            "Add",
            "Subtract",
            "Multiply",
            "Divide",
            "Min",
            "Max");

        struct OverNode::Private
        {
            std::shared_ptr<ftk::gl::Shader> shader;
        };

        void OverNode::_init(const std::shared_ptr<ftk::Context>& context)
        {
            NodeAttr attr;
            attr["Mode"] = OverMode::Premult;
            attr["Offset"] = ftk::V2I();
            INode::_init(context, getNodeInfo(), 2, 1, attr);
        }

        OverNode::OverNode() :
            _p(new Private)
        {}

        OverNode::~OverNode()
        {}

        NodeInfo OverNode::getNodeInfo()
        {
            return { "Over", "Over", "Compositing" };
        }

        std::shared_ptr<INode> OverNode::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            std::shared_ptr<OverNode> out(new OverNode);
            out->_init(context);
            return out;
        }

        namespace
        {
            const std::string vertexSource =
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

            const std::string fragmentSource =
                "#version 410\n"
                "\n"
                "in vec2 fTexture;\n"
                "out vec4 outColor;\n"
                "\n"
                "// enum ibis::render::OverMode\n"
                "const uint OverMode_Premult    = 0;\n"
                "const uint OverMode_NonPremult = 1;\n"
                "const uint OverMode_Add        = 2;\n"
                "const uint OverMode_Subtract   = 3;\n"
                "const uint OverMode_Multiply   = 4;\n"
                "const uint OverMode_Divide     = 5;\n"
                "const uint OverMode_Min        = 6;\n"
                "const uint OverMode_Max        = 7;\n"
                "\n"
                "uniform int overMode;\n"
                "uniform sampler2D textureSampler;\n"
                "uniform vec2 textureSamplerU;\n"
                "uniform vec2 textureSamplerV;\n"
                "uniform sampler2D textureSampler2;\n"
                "uniform vec2 textureSampler2U;\n"
                "uniform vec2 textureSampler2V;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    vec2 bgUV = fTexture;\n"
                "    bgUV.x = (bgUV.x - textureSamplerU.x) / (textureSamplerU.y - textureSamplerU.x);\n"
                "    bgUV.y = (bgUV.y - textureSamplerV.x) / (textureSamplerV.y - textureSamplerV.x);\n"
                "    vec4 bg = vec4(0.0, 0.0, 0.0, 0.0);\n"
                "    if (bgUV.x >= 0.0 && bgUV.x <= 1.0 && bgUV.y >= 0.0 && bgUV.y <= 1.0)\n"
                "        bg = texture(textureSampler, bgUV);\n"
                "    vec2 fgUV = fTexture;\n"
                "    fgUV.x = (fgUV.x - textureSampler2U.x) / (textureSampler2U.y - textureSampler2U.x);\n"
                "    fgUV.y = (fgUV.y - textureSampler2V.x) / (textureSampler2V.y - textureSampler2V.x);\n"
                "    vec4 fg = vec4(0.0, 0.0, 0.0, 0.0);\n"
                "    if (fgUV.x >= 0.0 && fgUV.x <= 1.0 && fgUV.y >= 0.0 && fgUV.y <= 1.0)\n"
                "        fg = texture(textureSampler2, fgUV);\n"
                "\n"
                "    if (OverMode_Premult == overMode)\n"
                "    {\n"
                "        float ia = 1.0 - fg.a;\n"
                "        outColor.r = fg.r + bg.r * ia;\n"
                "        outColor.g = fg.g + bg.g * ia;\n"
                "        outColor.b = fg.b + bg.b * ia;\n"
                "        outColor.a = clamp(fg.a + ia, 0.0, 1.0);\n"
                "    }\n"
                "    else if (OverMode_NonPremult == overMode)\n"
                "    {\n"
                "        float ia = 1.0 - fg.a;\n"
                "        outColor.r = fg.r * fg.a + bg.r * ia;\n"
                "        outColor.g = fg.g * fg.a + bg.g * ia;\n"
                "        outColor.b = fg.b * fg.a + bg.b * ia;\n"
                "        outColor.a = clamp(fg.a + ia, 0.0, 1.0);\n"
                "    }\n"
                "    else if (OverMode_Add == overMode)\n"
                "    {\n"
                "        outColor.r = fg.r + bg.r;\n"
                "        outColor.g = fg.g + bg.g;\n"
                "        outColor.b = fg.b + bg.b;\n"
                "        outColor.a = max(fg.a, bg.a);\n"
                "    }\n"
                "    else if (OverMode_Subtract == overMode)\n"
                "    {\n"
                "        outColor.r = bg.r - fg.r;\n"
                "        outColor.g = bg.g - fg.g;\n"
                "        outColor.b = bg.b - fg.b;\n"
                "        outColor.a = max(fg.a, bg.a);\n"
                "    }\n"
                "    else if (OverMode_Multiply == overMode)\n"
                "    {\n"
                "        outColor.r = fg.r * bg.r;\n"
                "        outColor.g = fg.g * bg.g;\n"
                "        outColor.b = fg.b * bg.b;\n"
                "        outColor.a = max(fg.a, bg.a);\n"
                "    }\n"
                "    else if (OverMode_Divide == overMode)\n"
                "    {\n"
                "        outColor.r = bg.r / fg.r;\n"
                "        outColor.g = bg.g / fg.g;\n"
                "        outColor.b = bg.b / fg.b;\n"
                "        outColor.a = max(fg.a, bg.a);\n"
                "    }\n"
                "    else if (OverMode_Min == overMode)\n"
                "    {\n"
                "        outColor.r = min(fg.r, bg.r);\n"
                "        outColor.g = min(fg.g, bg.g);\n"
                "        outColor.b = min(fg.b, bg.b);\n"
                "        outColor.a = min(fg.a, bg.a);\n"
                "    }\n"
                "    else if (OverMode_Max == overMode)\n"
                "    {\n"
                "        outColor.r = max(fg.r, bg.r);\n"
                "        outColor.g = max(fg.g, bg.g);\n"
                "        outColor.b = max(fg.b, bg.b);\n"
                "        outColor.a = max(fg.a, bg.a);\n"
                "    }\n"
                "}\n";
        }

        void OverNode::exec(
            const std::shared_ptr<ftk::IRender>& render,
            const OTIO_NS::RationalTime& time)
        {
            INode::exec(render, time);
            FTK_P();

            if (!p.shader)
            {
                p.shader = ftk::gl::Shader::create(vertexSource, fragmentSource);
            }

            ftk::Size2I size;
            if (_inputs->getItem(0).node &&
                !_inputs->getItem(0).node->getOutputs().empty() &&
                _inputs->getItem(0).node->getOutputs().front() &&
                _inputs->getItem(1).node &&
                !_inputs->getItem(1).node->getOutputs().empty() &&
                _inputs->getItem(1).node->getOutputs().front())
            {
                const auto& input0 = _inputs->getItem(0).node->getOutputs();
                const auto& input1 = _inputs->getItem(1).node->getOutputs();
                const ftk::Size2I& size0 = input0.front()->getSize();
                const ftk::Size2I& size1 = input1.front()->getSize();
                const ftk::V2I pos = _attr->getItem("Offset");
                size.w = std::max(size0.w, pos.x + size1.w);
                size.h = std::max(size0.h, pos.y + size1.h);
                if (size.isValid())
                {
                    ftk::gl::OffscreenBufferOptions offscreenBufferOptions;
                    offscreenBufferOptions.color = ftk::ImageType::RGBA_F32;
                    if (ftk::gl::doCreate(_outputs[0], size, offscreenBufferOptions))
                    {
                        _outputs[0] = ftk::gl::OffscreenBuffer::create(size, offscreenBufferOptions);
                    }
                    ftk::gl::OffscreenBufferBinding binding(_outputs[0]);
                    render->setRenderSize(size);
                    render->setViewport(ftk::Box2I(0, 0, size.w, size.h));
                    render->clearViewport(ftk::Color4F(0.F, 0.F, 0.F, 0.F));
                    p.shader->bind();
                    const auto pm = ftk::ortho(
                        0.F,
                        static_cast<float>(size.w),
                        static_cast<float>(size.h),
                        0.F,
                        -1.F,
                        1.F);
                    p.shader->setUniform("transform.mvp", pm);
                    p.shader->setUniform("overMode", static_cast<int>(_attr->getItem("Mode")));
                    p.shader->setUniform("textureSampler", 0);
                    p.shader->setUniform("textureSamplerU", ftk::V2F(0.0, size0.w / static_cast<float>(size.w)));
                    p.shader->setUniform("textureSamplerV", ftk::V2F(0.0, size0.h / static_cast<float>(size.h)));
                    p.shader->setUniform("textureSampler2", 1);
                    p.shader->setUniform("textureSampler2U", ftk::V2F(
                        pos.x / static_cast<float>(size.w),
                        (pos.x + size1.w) / static_cast<float>(size.w)));
                    p.shader->setUniform("textureSampler2V", ftk::V2F(
                        pos.y / static_cast<float>(size.h),
                        (pos.y + size1.h) / static_cast<float>(size.h)));

                    glActiveTexture(static_cast<GLenum>(GL_TEXTURE0));
                    glBindTexture(GL_TEXTURE_2D, input0.front()->getColorID());
                    glActiveTexture(static_cast<GLenum>(GL_TEXTURE1));
                    glBindTexture(GL_TEXTURE_2D, input1.front()->getColorID());

                    auto vbo = ftk::gl::VBO::create(2 * 3, ftk::gl::VBOType::Pos2_F32_UV_U16);
                    vbo->copy(convert(mesh(ftk::Box2I(0, 0, size.w, size.h), true), vbo->getType()));
                    auto vao = ftk::gl::VAO::create(vbo->getType(), vbo->getID());
                    vao->bind();
                    vao->draw(GL_TRIANGLES, 0, vbo->getSize());
                }
            }
            else if (_inputs->getItem(0).node &&
                !_inputs->getItem(0).node->getOutputs().empty() &&
                _inputs->getItem(0).node->getOutputs().front())
            {
                const auto& input = _inputs->getItem(0).node->getOutputs();
                size = input.front()->getSize();
                if (size.isValid())
                {
                    ftk::gl::OffscreenBufferOptions offscreenBufferOptions;
                    offscreenBufferOptions.color = ftk::ImageType::RGBA_F32;
                    if (ftk::gl::doCreate(_outputs[0], size, offscreenBufferOptions))
                    {
                        _outputs[0] = ftk::gl::OffscreenBuffer::create(size, offscreenBufferOptions);
                    }
                    ftk::gl::OffscreenBufferBinding binding(_outputs[0]);
                    render->setRenderSize(size);
                    render->setViewport(ftk::Box2I(0, 0, size.w, size.h));
                    render->clearViewport(ftk::Color4F(0.F, 0.F, 0.F, 0.F));
                    const auto pm = ftk::ortho(
                        0.F,
                        static_cast<float>(size.w),
                        static_cast<float>(size.h),
                        0.F,
                        -1.F,
                        1.F);
                    render->setTransform(pm);
                    render->drawTexture(
                        input.front()->getColorID(),
                        ftk::Box2I(0, 0, size.w, size.h),
                        true);
                }
            }
            else if (_inputs->getItem(1).node &&
                !_inputs->getItem(1).node->getOutputs().empty() &&
                _inputs->getItem(1).node->getOutputs().front())
            {
                const auto& input = _inputs->getItem(1).node->getOutputs();
                size = input.front()->getSize();
                if (size.isValid())
                {
                    ftk::gl::OffscreenBufferOptions offscreenBufferOptions;
                    offscreenBufferOptions.color = ftk::ImageType::RGBA_F32;
                    if (ftk::gl::doCreate(_outputs[0], size, offscreenBufferOptions))
                    {
                        _outputs[0] = ftk::gl::OffscreenBuffer::create(size, offscreenBufferOptions);
                    }
                    ftk::gl::OffscreenBufferBinding binding(_outputs[0]);
                    render->setRenderSize(size);
                    render->setViewport(ftk::Box2I(0, 0, size.w, size.h));
                    render->clearViewport(ftk::Color4F(0.F, 0.F, 0.F, 0.F));
                    const auto pm = ftk::ortho(
                        0.F,
                        static_cast<float>(size.w),
                        static_cast<float>(size.h),
                        0.F,
                        -1.F,
                        1.F);
                    render->setTransform(pm);
                    render->drawTexture(
                        input.front()->getColorID(),
                        ftk::Box2I(0, 0, size.w, size.h),
                        true);
                }
            }
            else
            {
                _outputs[0].reset();
            }
            if (!size.isValid())
            {
                _outputs[0].reset();
            }
        }
    }
}
