// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "CompNode.h"

#include <ftk/GL/GL.h>
#include <ftk/GL/Mesh.h>
#include <ftk/GL/OffscreenBuffer.h>
#include <ftk/GL/Shader.h>
#include <ftk/Core/IRender.h>

namespace ibis
{
    namespace render
    {
        struct OverNode::Private
        {
            std::shared_ptr<ftk::gl::Shader> shader;
        };

        void OverNode::_init(const std::shared_ptr<ftk::Context>& context)
        {
            INode::_init(context, getNodeInfo(), 2);
            FTK_P();
        }

        OverNode::OverNode() :
            _p(new Private)
        {}

        OverNode::~OverNode()
        {}

        NodeInfo OverNode::getNodeInfo()
        {
            return { "Over", "Over", "Compositing", "OverNode" };
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
                "    bgUV.x = bgUV.x * (textureSamplerU.y - textureSamplerU.x) - textureSamplerU.x;\n"
                "    bgUV.y = bgUV.y * (textureSamplerV.y - textureSamplerV.x) - textureSamplerV.x;\n"
                "    vec4 bg = vec4(0.0, 0.0, 0.0, 0.0);\n"
                "    if (bgUV.x >= 0.0 && bgUV.x <= 1.0 && bgUV.y >= 0.0 && bgUV.y <= 1.0)\n"
                "        bg = texture(textureSampler, bgUV);\n"
                "    vec2 fgUV = fTexture;\n"
                "    fgUV.x = fgUV.x * (textureSampler2U.y - textureSampler2U.x) - textureSampler2U.x;\n"
                "    fgUV.y = fgUV.y * (textureSampler2V.y - textureSampler2V.x) - textureSampler2V.x;\n"
                "    vec4 fg = vec4(0.0, 0.0, 0.0, 0.0);\n"
                "    if (fgUV.x >= 0.0 && fgUV.x <= 1.0 && fgUV.y >= 0.0 && fgUV.y <= 1.0)\n"
                "        fg = texture(textureSampler2, fgUV);\n"
                "    float ia = 1.0 - fg.a;\n"
                "    outColor.r = fg.r * fg.a + bg.r * ia;\n"
                "    outColor.g = fg.g * fg.a + bg.g * ia;\n"
                "    outColor.b = fg.b * fg.a + bg.b * ia;\n"
                "    outColor.a = min(fg.a + ia, 1.0);\n"
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
                size.w = std::max(size0.w, size1.w);
                size.h = std::max(size0.h, size1.h);
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
                    p.shader->setUniform("textureSampler", 0);
                    p.shader->setUniform("textureSamplerU", ftk::V2F(0.0, size.w / static_cast<float>(size0.w)));
                    p.shader->setUniform("textureSamplerV", ftk::V2F(0.0, size.h / static_cast<float>(size0.h)));
                    p.shader->setUniform("textureSampler2", 1);
                    p.shader->setUniform("textureSampler2U", ftk::V2F(0.0, size.w / static_cast<float>(size1.w)));
                    p.shader->setUniform("textureSampler2V", ftk::V2F(0.0, size.h / static_cast<float>(size1.h)));

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
