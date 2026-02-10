// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "OutputNode.h"

#include <ibis/Core/Time.h>

#include <ftk/GL/GL.h>
#include <ftk/GL/Util.h>
#include <ftk/Core/Error.h>
#include <ftk/Core/Format.h>
#include <ftk/Core/IRender.h>
#include <ftk/Core/Path.h>
#include <ftk/Core/String.h>

#include <OpenImageIO/filesystem.h>
#include <OpenImageIO/imagebufalgo.h>

namespace ibis
{
    namespace render
    {
        struct ImageOutputNode::Private
        {
        };

        void ImageOutputNode::_init(
            const std::shared_ptr<ftk::Context>& context,
            const nlohmann::json& json)
        {
            NodeAttr attr;
            attr["Dir"] = "";
            attr["BaseName"] = "";
            attr["Ext"] = ".png";
            IOutputNode::_init(context, getClassNodeInfo(), 1, 1, attr, json);
        }

        ImageOutputNode::ImageOutputNode() :
            _p(new Private)
        {}

        ImageOutputNode::~ImageOutputNode()
        {}

        NodeInfo ImageOutputNode::getClassNodeInfo()
        {
            return { "ImageOutput", "Image Output", "I/O" };
        }

        std::vector<std::string> ImageOutputNode::getExts()
        {
            std::vector<std::string> out;
            for (const auto& i : OIIO::get_extension_map())
            {
                // Filter out FFmpeg extensions.
                if (i.first != "ffmpeg")
                {
                    for (const auto& ext : i.second)
                    {
                        out.push_back("." + ext);
                    }
                }
            }
            return out;
        }

        std::shared_ptr<INode> ImageOutputNode::create(
            const std::shared_ptr<ftk::Context>& context,
            const nlohmann::json& json)
        {
            std::shared_ptr<ImageOutputNode> out(new ImageOutputNode);
            out->_init(context, json);
            return out;
        }

        namespace
        {
            OIIO::TypeDesc toOIIO(ftk::ImageType value)
            {
                OIIO::TypeDesc out;
                switch (value)
                {
                case ftk::ImageType::L_U8:
                case ftk::ImageType::LA_U8:
                case ftk::ImageType::RGB_U8:
                case ftk::ImageType::RGBA_U8:
                    out = OIIO::TypeDesc::UINT8;
                    break;
                case ftk::ImageType::L_U16:
                case ftk::ImageType::LA_U16:
                case ftk::ImageType::RGB_U16:
                case ftk::ImageType::RGBA_U16:
                    out = OIIO::TypeDesc::UINT16;
                    break;
                case ftk::ImageType::L_U32:
                case ftk::ImageType::LA_U32:
                case ftk::ImageType::RGB_U32:
                case ftk::ImageType::RGBA_U32:
                    out = OIIO::TypeDesc::UINT32;
                    break;
                case ftk::ImageType::L_F16:
                case ftk::ImageType::LA_F16:
                case ftk::ImageType::RGB_F16:
                case ftk::ImageType::RGBA_F16:
                    out = OIIO::TypeDesc::HALF;
                    break;
                case ftk::ImageType::L_F32:
                case ftk::ImageType::LA_F32:
                case ftk::ImageType::RGB_F32:
                case ftk::ImageType::RGBA_F32:
                    out = OIIO::TypeDesc::FLOAT;
                    break;
                default: break;
                }
                return out;
            }
        }

        void ImageOutputNode::write(const OTIO_NS::RationalTime&)
        {
            FTK_P();
            if (_outputs[0])
            {
                const ftk::ImageType type = ftk::gl::getImageType(_outputs[0]->getType());
                if (type != ftk::ImageType::None)
                {
                    auto image = ftk::Image::create(_outputs[0]->getSize(), type);
                    {
                        ftk::gl::OffscreenBufferBinding binding(_outputs[0]);
                        glPixelStorei(GL_PACK_ALIGNMENT, 1);
#if defined(FTK_API_GL_4_1)
                        glClampColor(GL_CLAMP_READ_COLOR, GL_FALSE);
#endif // FTK_API_GL_4_1
                        glReadPixels(
                            0,
                            0,
                            _outputs[0]->getWidth(),
                            _outputs[0]->getHeight(),
                            ftk::gl::getReadPixelsFormat(image->getType()),
                            ftk::gl::getReadPixelsType(image->getType()),
                            image->getData());
                    }
                    const ftk::Path path(
                        static_cast<std::string>(_attr->getItem("Dir")),
                        static_cast<std::string>(_attr->getItem("BaseName")) +
                        static_cast<std::string>(_attr->getItem("Ext")));
                    auto oiioOutput = OIIO::ImageOutput::create(path.get());
                    if (!oiioOutput)
                    {
                        throw std::runtime_error(OIIO::geterror());
                    }
                    const auto& info = image->getInfo();
                    OIIO::ImageSpec oiioSpec(
                        info.size.w,
                        info.size.h,
                        ftk::getChannelCount(info.type),
                        toOIIO(info.type));
                    for (const auto& tag : image->getTags())
                    {
                        oiioSpec.attribute(tag.first, tag.second);
                    }
                    if (!oiioOutput->open(path.get(), oiioSpec))
                    {
                        throw std::runtime_error(OIIO::geterror());
                    }
                    const size_t scanlineByteCount = oiioSpec.scanline_bytes();
                    if (!oiioOutput->write_image(
                        oiioSpec.format,
                        image->getData() + (info.size.h - 1) * scanlineByteCount,
                        OIIO::AutoStride,
                        -scanlineByteCount,
                        OIIO::AutoStride))
                    {
                        throw std::runtime_error(OIIO::geterror());
                    }
                }
            }
        }

        void ImageOutputNode::exec(
            const std::shared_ptr<ftk::IRender>& render,
            const OTIO_NS::RationalTime& time)
        {
            IOutputNode::exec(render, time);
            FTK_P();

            ftk::gl::TextureInfo info;
            if (_inputs->getItem(0).node)
            {
                const auto& input0 = _inputs->getItem(0).node->getOutputs();
                if (!input0.empty() && input0.front())
                {
                    info.size = input0.front()->getSize();
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
            _outputInfo->setItemOnlyIfChanged(0, info);
        }

        struct SequenceOutputNode::Private
        {
        };

        void SequenceOutputNode::_init(
            const std::shared_ptr<ftk::Context>& context,
            const nlohmann::json& json)
        {
            NodeAttr attr;
            attr["Dir"] = "";
            attr["BaseName"] = "";
            attr["Pad"] = 4;
            attr["Ext"] = ".png";
            IOutputNode::_init(context, getClassNodeInfo(), 1, 1, attr, json);
        }

        SequenceOutputNode::SequenceOutputNode() :
            _p(new Private)
        {}

        SequenceOutputNode::~SequenceOutputNode()
        {}

        NodeInfo SequenceOutputNode::getClassNodeInfo()
        {
            return { "SequenceOutput", "Sequence Output", "I/O" };
        }

        std::vector<std::string> SequenceOutputNode::getExts()
        {
            return ImageOutputNode::getExts();
        }

        std::shared_ptr<INode> SequenceOutputNode::create(
            const std::shared_ptr<ftk::Context>& context,
            const nlohmann::json& json)
        {
            std::shared_ptr<SequenceOutputNode> out(new SequenceOutputNode);
            out->_init(context, json);
            return out;
        }

        void SequenceOutputNode::write(const OTIO_NS::RationalTime& time)
        {
            FTK_P();
            if (_outputs[0])
            {
                const ftk::ImageType type = ftk::gl::getImageType(_outputs[0]->getType());
                if (type != ftk::ImageType::None)
                {
                    auto image = ftk::Image::create(_outputs[0]->getSize(), type);
                    {
                        ftk::gl::OffscreenBufferBinding binding(_outputs[0]);
                        glPixelStorei(GL_PACK_ALIGNMENT, 1);
#if defined(FTK_API_GL_4_1)
                        glClampColor(GL_CLAMP_READ_COLOR, GL_FALSE);
#endif // FTK_API_GL_4_1
                        glReadPixels(
                            0,
                            0,
                            _outputs[0]->getWidth(),
                            _outputs[0]->getHeight(),
                            ftk::gl::getReadPixelsFormat(image->getType()),
                            ftk::gl::getReadPixelsType(image->getType()),
                            image->getData());
                    }
                    const ftk::Path path(
                        static_cast<std::string>(_attr->getItem("Dir")),
                        static_cast<std::string>(_attr->getItem("BaseName")) +
                        ftk::toString(
                            static_cast<int64_t>(time.value()),
                            static_cast<int>(_attr->getItem("Pad"))) +
                        static_cast<std::string>(_attr->getItem("Ext")));
                    const std::string fileName = path.getFrame(time.value(), true);
                    auto oiioOutput = OIIO::ImageOutput::create(fileName);
                    if (!oiioOutput)
                    {
                        throw std::runtime_error(OIIO::geterror());
                    }
                    const auto& info = image->getInfo();
                    OIIO::ImageSpec oiioSpec(
                        info.size.w,
                        info.size.h,
                        ftk::getChannelCount(info.type),
                        toOIIO(info.type));
                    for (const auto& tag : image->getTags())
                    {
                        oiioSpec.attribute(tag.first, tag.second);
                    }
                    if (!oiioOutput->open(fileName, oiioSpec))
                    {
                        throw std::runtime_error(OIIO::geterror());
                    }
                    const size_t scanlineByteCount = oiioSpec.scanline_bytes();
                    if (!oiioOutput->write_image(
                        oiioSpec.format,
                        image->getData() + (info.size.h - 1) * scanlineByteCount,
                        OIIO::AutoStride,
                        -scanlineByteCount,
                        OIIO::AutoStride))
                    {
                        throw std::runtime_error(OIIO::geterror());
                    }
                }
            }
        }

        void SequenceOutputNode::exec(
            const std::shared_ptr<ftk::IRender>& render,
            const OTIO_NS::RationalTime& time)
        {
            IOutputNode::exec(render, time);
            FTK_P();

            ftk::gl::TextureInfo info;
            if (_inputs->getItem(0).node)
            {
                const auto& input0 = _inputs->getItem(0).node->getOutputs();
                if (!input0.empty() && input0.front())
                {
                    info.size = input0.front()->getSize();
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
            _outputInfo->setItemOnlyIfChanged(0, info);
        }
    }
}
