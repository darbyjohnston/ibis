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
            const std::string colorMatrixVertex =
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

            const std::string colorMatrixFragment =
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
                "    outColor = colorMatrix * texture(textureSampler, fTexture);\n"
                "}\n";
        }

        struct IColorMatrixNode::Private
        {
            std::shared_ptr<ftk::gl::Shader> shader;
        };

        void IColorMatrixNode::_init(
            const std::shared_ptr<ftk::Context>& context,
            const NodeInfo& info,
            int inputCount,
            int outputCount,
            const NodeAttr& attr)
        {
            INode::_init(context, info, inputCount, outputCount, attr);
        }

        IColorMatrixNode::IColorMatrixNode() :
            _p(new Private)
        {}

        IColorMatrixNode::~IColorMatrixNode()
        {}

        void IColorMatrixNode::exec(
            const std::shared_ptr<ftk::IRender>& render,
            const OTIO_NS::RationalTime& time)
        {
            INode::exec(render, time);
            FTK_P();

            if (!p.shader)
            {
                p.shader = ftk::gl::Shader::create(colorMatrixVertex, colorMatrixFragment);
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
                    p.shader->setUniform("colorMatrix", _getColorMatrix());
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

            _imageInfo->setItemOnlyIfChanged(
                0,
                _outputs[0] ?
                ftk::ImageInfo(_outputs[0]->getWidth(), _outputs[0]->getHeight(), _outputs[0]->getOptions().color) :
                ftk::ImageInfo());
        }

        void BrightnessNode::_init(const std::shared_ptr<ftk::Context>& context)
        {
            NodeAttr attr;
            attr["Value"] = 1.0;
            IColorMatrixNode::_init(context, getClassNodeInfo(), 1, 1, attr);
        }

        BrightnessNode::~BrightnessNode()
        {}

        NodeInfo BrightnessNode::getClassNodeInfo()
        {
            return { "Brightness", "Brightness", "Color" };
        }

        std::shared_ptr<INode> BrightnessNode::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            std::shared_ptr<BrightnessNode> out(new BrightnessNode);
            out->_init(context);
            return out;
        }

        ftk::M44F BrightnessNode::_getColorMatrix() const
        {
            const float v = _attr->getItem("Value");
            return ftk::brightness(ftk::V3F(v, v, v));
        }

        void ContrastNode::_init(const std::shared_ptr<ftk::Context>& context)
        {
            NodeAttr attr;
            attr["Value"] = 1.0;
            IColorMatrixNode::_init(context, getClassNodeInfo(), 1, 1, attr);
        }

        ContrastNode::~ContrastNode()
        {}

        NodeInfo ContrastNode::getClassNodeInfo()
        {
            return { "Contrast", "Contrast", "Color" };
        }

        std::shared_ptr<INode> ContrastNode::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            std::shared_ptr<ContrastNode> out(new ContrastNode);
            out->_init(context);
            return out;
        }

        ftk::M44F ContrastNode::_getColorMatrix() const
        {
            const float v = _attr->getItem("Value");
            return ftk::contrast(ftk::V3F(v, v, v));
        }

        void SaturationNode::_init(const std::shared_ptr<ftk::Context>& context)
        {
            NodeAttr attr;
            attr["Value"] = 1.0;
            IColorMatrixNode::_init(context, getClassNodeInfo(), 1, 1, attr);
        }

        SaturationNode::~SaturationNode()
        {}

        NodeInfo SaturationNode::getClassNodeInfo()
        {
            return { "Saturation", "Saturation", "Color" };
        }

        std::shared_ptr<INode> SaturationNode::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            std::shared_ptr<SaturationNode> out(new SaturationNode);
            out->_init(context);
            return out;
        }

        ftk::M44F SaturationNode::_getColorMatrix() const
        {
            const float v = _attr->getItem("Value");
            return ftk::saturation(ftk::V3F(v, v, v));
        }

        void TintNode::_init(const std::shared_ptr<ftk::Context>& context)
        {
            NodeAttr attr;
            attr["Value"] = 0.0;
            IColorMatrixNode::_init(context, getClassNodeInfo(), 1, 1, attr);
        }

        TintNode::~TintNode()
        {}

        NodeInfo TintNode::getClassNodeInfo()
        {
            return { "Tint", "Tint", "Color" };
        }

        std::shared_ptr<INode> TintNode::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            std::shared_ptr<TintNode> out(new TintNode);
            out->_init(context);
            return out;
        }

        ftk::M44F TintNode::_getColorMatrix() const
        {
            const float v = _attr->getItem("Value") / 360.F;
            return ftk::tint(v);
        }

        struct InvertNode::Private
        {
            std::shared_ptr<ftk::gl::Shader> shader;
        };

        void InvertNode::_init(const std::shared_ptr<ftk::Context>& context)
        {
            INode::_init(context, getClassNodeInfo(), 1);
        }

        InvertNode::InvertNode() :
            _p(new Private)
        {}

        InvertNode::~InvertNode()
        {}

        NodeInfo InvertNode::getClassNodeInfo()
        {
            return { "Invert", "Invert", "Color" };
        }

        std::shared_ptr<INode> InvertNode::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            std::shared_ptr<InvertNode> out(new InvertNode);
            out->_init(context);
            return out;
        }

        namespace
        {
            const std::string invertVertex =
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

            const std::string invertFragment =
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
                "    outColor.r = 1.0 - outColor.r;\n"
                "    outColor.g = 1.0 - outColor.g;\n"
                "    outColor.b = 1.0 - outColor.b;\n"
                "}\n";
        }

        void InvertNode::exec(
            const std::shared_ptr<ftk::IRender>& render,
            const OTIO_NS::RationalTime& time)
        {
            INode::exec(render, time);
            FTK_P();

            if (!p.shader)
            {
                p.shader = ftk::gl::Shader::create(invertVertex, invertFragment);
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

            _imageInfo->setItemOnlyIfChanged(
                0,
                _outputs[0] ?
                ftk::ImageInfo(_outputs[0]->getWidth(), _outputs[0]->getHeight(), _outputs[0]->getOptions().color) :
                ftk::ImageInfo());
        }

        struct LevelsNode::Private
        {
            std::shared_ptr<ftk::gl::Shader> shader;
        };

        void LevelsNode::_init(const std::shared_ptr<ftk::Context>& context)
        {
            NodeAttr attr;
            attr["InLow"] = 0.0;
            attr["InHigh"] = 1.0;
            attr["Gamma"] = 1.0;
            attr["OutLow"] = 0.0;
            attr["OutHigh"] = 1.0;
            INode::_init(context, getClassNodeInfo(), 1, 1, attr);
        }

        LevelsNode::LevelsNode() :
            _p(new Private)
        {}

        LevelsNode::~LevelsNode()
        {}

        NodeInfo LevelsNode::getClassNodeInfo()
        {
            return { "Levels", "Levels", "Color" };
        }

        std::shared_ptr<INode> LevelsNode::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            std::shared_ptr<LevelsNode> out(new LevelsNode);
            out->_init(context);
            return out;
        }

        namespace
        {
            const std::string levelsVertex =
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

            const std::string levelsFragment =
                "#version 410\n"
                "\n"
                "in vec2 fTexture;\n"
                "out vec4 outColor;\n"
                "\n"
                "struct Levels\n"
                "{\n"
                "    float inLow;\n"
                "    float inHigh;\n"
                "    float gamma;\n"
                "    float outLow;\n"
                "    float outHigh;\n"
                "};\n"
                "uniform Levels levels;\n"
                "uniform sampler2D textureSampler;\n"
                "\n"
                "vec4 levelsFunc(vec4 value, Levels data)\n"
                "{\n"
                "    vec4 tmp;\n"
                "    tmp[0] = (value[0] - data.inLow) / data.inHigh;\n"
                "    tmp[1] = (value[1] - data.inLow) / data.inHigh;\n"
                "    tmp[2] = (value[2] - data.inLow) / data.inHigh;\n"
                "    if (tmp[0] >= 0.0)\n"
                "        tmp[0] = pow(tmp[0], data.gamma);\n"
                "    if (tmp[1] >= 0.0)\n"
                "        tmp[1] = pow(tmp[1], data.gamma);\n"
                "    if (tmp[2] >= 0.0)\n"
                "        tmp[2] = pow(tmp[2], data.gamma);\n"
                "    value[0] = tmp[0] * data.outHigh + data.outLow;\n"
                "    value[1] = tmp[1] * data.outHigh + data.outLow;\n"
                "    value[2] = tmp[2] * data.outHigh + data.outLow;\n"
                "    return value;\n"
                "}\n"
                "\n"
                "void main()\n"
                "{\n"
                "    outColor = levelsFunc(texture(textureSampler, fTexture), levels);\n"
                "}\n";
        }

        void LevelsNode::exec(
            const std::shared_ptr<ftk::IRender>& render,
            const OTIO_NS::RationalTime& time)
        {
            INode::exec(render, time);
            FTK_P();

            if (!p.shader)
            {
                p.shader = ftk::gl::Shader::create(levelsVertex, levelsFragment);
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
                    p.shader->setUniform("levels.inLow", float(_attr->getItem("InLow")));
                    p.shader->setUniform("levels.inHigh", float(_attr->getItem("InHigh")));
                    const float gamma = _attr->getItem("Gamma");
                    p.shader->setUniform("levels.gamma", gamma > 0.F ? (1.F / gamma) : 1000000.F);
                    p.shader->setUniform("levels.outLow", float(_attr->getItem("OutLow")));
                    p.shader->setUniform("levels.outHigh", float(_attr->getItem("OutHigh")));
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

            _imageInfo->setItemOnlyIfChanged(
                0,
                _outputs[0] ?
                ftk::ImageInfo(_outputs[0]->getWidth(), _outputs[0]->getHeight(), _outputs[0]->getOptions().color) :
                ftk::ImageInfo());
        }

        struct SoftClipNode::Private
        {
            std::shared_ptr<ftk::gl::Shader> shader;
        };

        void SoftClipNode::_init(const std::shared_ptr<ftk::Context>& context)
        {
            NodeAttr attr;
            attr["Value"] = 0.0;
            INode::_init(context, getClassNodeInfo(), 1, 1, attr);
        }

        SoftClipNode::SoftClipNode() :
            _p(new Private)
        {}

        SoftClipNode::~SoftClipNode()
        {}

        NodeInfo SoftClipNode::getClassNodeInfo()
        {
            return { "SoftClip", "SoftClip", "Color" };
        }

        std::shared_ptr<INode> SoftClipNode::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            std::shared_ptr<SoftClipNode> out(new SoftClipNode);
            out->_init(context);
            return out;
        }

        namespace
        {
            const std::string softClipVertex =
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

            const std::string softClipFragment =
                "#version 410\n"
                "\n"
                "in vec2 fTexture;\n"
                "out vec4 outColor;\n"
                "\n"
                "uniform float value;\n"
                "uniform sampler2D textureSampler;\n"
                "\n"
                "vec4 softClipFunc(vec4 value, float softClip)\n"
                "{\n"
                "    float tmp = 1.0 - softClip;\n"
                "    if (value[0] > tmp)\n"
                "        value[0] = tmp + (1.0 - exp(-(value[0] - tmp) / softClip)) * softClip;\n"
                "    if (value[1] > tmp)\n"
                "        value[1] = tmp + (1.0 - exp(-(value[1] - tmp) / softClip)) * softClip;\n"
                "    if (value[2] > tmp)\n"
                "        value[2] = tmp + (1.0 - exp(-(value[2] - tmp) / softClip)) * softClip;\n"
                "    return value;\n"
                "}\n"
                "\n"
                "void main()\n"
                "{\n"
                "    outColor = softClipFunc(texture(textureSampler, fTexture), value);\n"
                "}\n";
        }

        void SoftClipNode::exec(
            const std::shared_ptr<ftk::IRender>& render,
            const OTIO_NS::RationalTime& time)
        {
            INode::exec(render, time);
            FTK_P();

            if (!p.shader)
            {
                p.shader = ftk::gl::Shader::create(softClipVertex, softClipFragment);
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

            _imageInfo->setItemOnlyIfChanged(
                0,
                _outputs[0] ?
                ftk::ImageInfo(_outputs[0]->getWidth(), _outputs[0]->getHeight(), _outputs[0]->getOptions().color) :
                ftk::ImageInfo());
        }
    }
}
