// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "ColorNode.h"

#include <ftk/GL/GL.h>
#include <ftk/GL/Mesh.h>
#include <ftk/GL/OffscreenBuffer.h>
#include <ftk/GL/Shader.h>
#include <ftk/Core/IRender.h>

namespace ibis
{
    namespace render
    {
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
                "uniform mat4 colorMatrix;\n"
                "uniform sampler2D textureSampler;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    outColor = texture(textureSampler, fTexture) * colorMatrix;\n"
                "}\n";
        }

        struct BrightnessNode::Private
        {
            std::shared_ptr<ftk::gl::Shader> shader;
        };

        void BrightnessNode::_init(const std::shared_ptr<ftk::Context>& context)
        {
            NodeAttr attr;
            attr["Value"] = 1.0;
            INode::_init(context, getNodeInfo(), 1, 1, attr);
            FTK_P();
        }

        BrightnessNode::BrightnessNode() :
            _p(new Private)
        {}

        BrightnessNode::~BrightnessNode()
        {}

        NodeInfo BrightnessNode::getNodeInfo()
        {
            return { "Brightness", "Brightness", "Color", "BrightnessNode" };
        }

        std::shared_ptr<INode> BrightnessNode::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            std::shared_ptr<BrightnessNode> out(new BrightnessNode);
            out->_init(context);
            return out;
        }

        void BrightnessNode::exec(
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
            if (_inputs->getItem(0).node)
            {
                const auto& input0 = _inputs->getItem(0).node->getOutputs();
                if (!input0.empty() && input0.front())
                {
                    size = input0.front()->getSize();
                }
                if (size.isValid())
                {
                    ftk::gl::OffscreenBufferOptions offscreenBufferOptions;
                    offscreenBufferOptions.color = ftk::ImageType::RGBA_F32;
                    if (ftk::gl::doCreate(_outputs[0], size, offscreenBufferOptions))
                    {
                        _outputs[0] = ftk::gl::OffscreenBuffer::create(size, offscreenBufferOptions);
                    }
                    ftk::gl::OffscreenBufferBinding binding(_outputs[0]);
                    render->clearViewport(ftk::Color4F(0.F, 0.F, 0.F, 0.F));
                    render->setRenderSize(size);
                    render->setViewport(ftk::Box2I(0, 0, size.w, size.h));
                    p.shader->bind();
                    const auto pm = ftk::ortho(
                        0.F,
                        static_cast<float>(size.w),
                        static_cast<float>(size.h),
                        0.F,
                        -1.F,
                        1.F);
                    p.shader->setUniform("transform.mvp", pm);
                    const float v = _attr->getItem("Value");
                    p.shader->setUniform("colorMatrix", brightness(ftk::V3F(v, v, v)));
                    p.shader->setUniform("textureSampler", 0);

                    glActiveTexture(static_cast<GLenum>(GL_TEXTURE0));
                    glBindTexture(GL_TEXTURE_2D, input0.front()->getColorID());

                    auto vbo = ftk::gl::VBO::create(2 * 3, ftk::gl::VBOType::Pos2_F32_UV_U16);
                    vbo->copy(convert(mesh(ftk::Box2I(0, 0, size.w, size.h), true), vbo->getType()));
                    auto vao = ftk::gl::VAO::create(vbo->getType(), vbo->getID());
                    vao->bind();
                    vao->draw(GL_TRIANGLES, 0, vbo->getSize());
                }
            }
            if (!_inputs->getItem(0).node || !size.isValid())
            {
                _outputs[0].reset();
            }
        }

        struct ContrastNode::Private
        {
            std::shared_ptr<ftk::gl::Shader> shader;
        };

        void ContrastNode::_init(const std::shared_ptr<ftk::Context>& context)
        {
            NodeAttr attr;
            attr["Value"] = 1.0;
            INode::_init(context, getNodeInfo(), 1, 1, attr);
            FTK_P();
        }

        ContrastNode::ContrastNode() :
            _p(new Private)
        {}

        ContrastNode::~ContrastNode()
        {}

        NodeInfo ContrastNode::getNodeInfo()
        {
            return { "Contrast", "Contrast", "Color", "ContrastNode" };
        }

        std::shared_ptr<INode> ContrastNode::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            std::shared_ptr<ContrastNode> out(new ContrastNode);
            out->_init(context);
            return out;
        }

        void ContrastNode::exec(
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
            if (_inputs->getItem(0).node)
            {
                const auto& input0 = _inputs->getItem(0).node->getOutputs();
                if (!input0.empty() && input0.front())
                {
                    size = input0.front()->getSize();
                }
                if (size.isValid())
                {
                    ftk::gl::OffscreenBufferOptions offscreenBufferOptions;
                    offscreenBufferOptions.color = ftk::ImageType::RGBA_F32;
                    if (ftk::gl::doCreate(_outputs[0], size, offscreenBufferOptions))
                    {
                        _outputs[0] = ftk::gl::OffscreenBuffer::create(size, offscreenBufferOptions);
                    }
                    ftk::gl::OffscreenBufferBinding binding(_outputs[0]);
                    render->clearViewport(ftk::Color4F(0.F, 0.F, 0.F, 0.F));
                    render->setRenderSize(size);
                    render->setViewport(ftk::Box2I(0, 0, size.w, size.h));
                    p.shader->bind();
                    const auto pm = ftk::ortho(
                        0.F,
                        static_cast<float>(size.w),
                        static_cast<float>(size.h),
                        0.F,
                        -1.F,
                        1.F);
                    p.shader->setUniform("transform.mvp", pm);
                    const float v = _attr->getItem("Value");
                    p.shader->setUniform("colorMatrix", contrast(ftk::V3F(v, v, v)));
                    p.shader->setUniform("textureSampler", 0);

                    glActiveTexture(static_cast<GLenum>(GL_TEXTURE0));
                    glBindTexture(GL_TEXTURE_2D, input0.front()->getColorID());

                    auto vbo = ftk::gl::VBO::create(2 * 3, ftk::gl::VBOType::Pos2_F32_UV_U16);
                    vbo->copy(convert(mesh(ftk::Box2I(0, 0, size.w, size.h), true), vbo->getType()));
                    auto vao = ftk::gl::VAO::create(vbo->getType(), vbo->getID());
                    vao->bind();
                    vao->draw(GL_TRIANGLES, 0, vbo->getSize());
                }
            }
            if (!_inputs->getItem(0).node || !size.isValid())
            {
                _outputs[0].reset();
            }
        }
    }
}
