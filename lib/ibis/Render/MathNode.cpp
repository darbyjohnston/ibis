// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "MathNode.h"

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
            ArithmeticOperator,
            "Add",
            "Subtract",
            "Multiply",
            "Divide");

        struct ArithmeticNode::Private
        {
            std::shared_ptr<ftk::gl::Shader> shader;
        };

        void ArithmeticNode::_init(const std::shared_ptr<ftk::Context>& context)
        {
            NodeAttr attr;
            attr["Value"] = 0.5;
            attr["Operator"] = ArithmeticOperator::Add;
            INode::_init(context, getClassNodeInfo(), 1, 1, attr);
        }

        ArithmeticNode::ArithmeticNode() :
            _p(new Private)
        {}

        ArithmeticNode::~ArithmeticNode()
        {}

        NodeInfo ArithmeticNode::getClassNodeInfo()
        {
            return { "Arithmetic", "Arithmetic", "Math" };
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
                "// enum ibis::render::ArithmeticOperator\n"
                "const uint ArithmeticOperator_Add      = 0;\n"
                "const uint ArithmeticOperator_Subtract = 1;\n"
                "const uint ArithmeticOperator_Multiply = 2;\n"
                "const uint ArithmeticOperator_Divide   = 3;\n"
                "\n"
                "uniform int arithmeticOperator;\n"
                "uniform float value;\n"
                "uniform sampler2D textureSampler;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    outColor = texture(textureSampler, fTexture);\n"
                "    if (ArithmeticOperator_Add == arithmeticOperator)"
                "    {\n"
                "        outColor.r += value;\n"
                "        outColor.g += value;\n"
                "        outColor.b += value;\n"
                "    }\n"
                "    else if (ArithmeticOperator_Subtract == arithmeticOperator)"
                "    {\n"
                "        outColor.r -= value;\n"
                "        outColor.g -= value;\n"
                "        outColor.b -= value;\n"
                "    }\n"
                "    else if (ArithmeticOperator_Multiply == arithmeticOperator)"
                "    {\n"
                "        outColor.r *= value;\n"
                "        outColor.g *= value;\n"
                "        outColor.b *= value;\n"
                "    }\n"
                "    else if (ArithmeticOperator_Divide == arithmeticOperator)"
                "    {\n"
                "        outColor.r /= value;\n"
                "        outColor.g /= value;\n"
                "        outColor.b /= value;\n"
                "    }\n"
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
                    render->setViewport(ftk::Box2I(0, 0, info.size.w, info.size.h));
                    render->clearViewport(ftk::Color4F(0.F, 0.F, 0.F, 0.F));
                    p.shader->bind();
                    p.shader->setUniform("transform.mvp", _getProjection(info.size));
                    p.shader->setUniform("arithmeticOperator", static_cast<int>(_attr->getItem("Operator")));
                    p.shader->setUniform("value", float(_attr->getItem("Value")));
                    p.shader->setUniform("textureSampler", 0);

                    glActiveTexture(static_cast<GLenum>(GL_TEXTURE0));
                    glBindTexture(GL_TEXTURE_2D, input0.front()->getColorID());

                    auto vbo = ftk::gl::VBO::create(2 * 3, ftk::gl::VBOType::Pos2_F32_UV_U16);
                    vbo->copy(convert(mesh(ftk::Box2I(0, 0, info.size.w, info.size.h), true), vbo->getType()));
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
    }
}
