// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "MathNode.h"

#include <ftk/GL/GL.h>
#include <ftk/GL/Mesh.h>
#include <ftk/GL/OffscreenBuffer.h>
#include <ftk/GL/Shader.h>
#include <ftk/Core/IRender.h>

namespace ibis
{
    namespace render
    {
        struct ArithmeticNode::Private
        {
            std::shared_ptr<ftk::gl::Shader> shader;
        };

        void ArithmeticNode::_init(const std::shared_ptr<ftk::Context>& context)
        {
            NodeAttr attr;
            attr["Value"] = 0.5;
            INode::_init(context, getNodeInfo(), 1, 1, attr);
            FTK_P();
        }

        ArithmeticNode::ArithmeticNode() :
            _p(new Private)
        {}

        ArithmeticNode::~ArithmeticNode()
        {}

        NodeInfo ArithmeticNode::getNodeInfo()
        {
            return { "Arithmetic", "Arithmetic", "Math", "ArithmeticNode" };
        }

        std::shared_ptr<INode> ArithmeticNode::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            std::shared_ptr<ArithmeticNode> out(new ArithmeticNode);
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
                "uniform float value;\n"
                "uniform sampler2D textureSampler;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    outColor = texture(textureSampler, fTexture);\n"
                "    outColor.r += value;\n"
                "    outColor.g += value;\n"
                "    outColor.b += value;\n"
                "}\n";
        }

        void ArithmeticNode::exec(
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
                    p.shader->setUniform("value", float(_attr->getItem("Value")));
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
