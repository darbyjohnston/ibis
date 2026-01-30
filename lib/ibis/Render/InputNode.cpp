// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "InputNode.h"

#include <ibis/Core/Time.h>

#include <ftk/GL/GL.h>
#include <ftk/GL/OffscreenBuffer.h>
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
        FTK_ENUM_IMPL(
            InputLoop,
            "None",
            "Loop",
            "Clamp");

        OTIO_NS::RationalTime getInputLoop(
            InputLoop loop,
            const OTIO_NS::RationalTime& time,
            const OTIO_NS::TimeRange& range)
        {
            OTIO_NS::RationalTime out = time;
            switch (loop)
            {
            case InputLoop::Loop:
            {
                const OTIO_NS::RationalTime duration = range.duration();
                if (duration.value() > 0.0)
                {
                    out = OTIO_NS::RationalTime(
                        static_cast<int64_t>(out.value()) % static_cast<int64_t>(duration.value()),
                        out.rate());
                }
                break;
            }
            case InputLoop::Clamp:
                out = range.clamped(out);
                break;
            default: break;
            }
            return out;
        }

        struct ImageFileNode::Private
        {
            std::string path;
            std::shared_ptr<ftk::Image> image;
        };

        void ImageFileNode::_init(const std::shared_ptr<ftk::Context>& context)
        {
            NodeAttr attr;
            attr["Path"] = "";
            INode::_init(context, getNodeInfo(), 0, 1, attr);
            FTK_P();
        }

        ImageFileNode::ImageFileNode() :
            _p(new Private)
        {}

        ImageFileNode::~ImageFileNode()
        {}

        NodeInfo ImageFileNode::getNodeInfo()
        {
            return { "ImageFile", "Image File", "Input", "ImageFileNode" };
        }

        std::shared_ptr<INode> ImageFileNode::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            std::shared_ptr<ImageFileNode> out(new ImageFileNode);
            out->_init(context);
            return out;
        }

        namespace
        {
            ftk::ImageType fromOIIO(const OIIO::ImageSpec& oiio)
            {
                ftk::ImageType out = ftk::ImageType::None;
                if (1 == oiio.nchannels)
                {
                    switch (oiio.format.basetype)
                    {
                    case OIIO::TypeDesc::UINT8:  out = ftk::ImageType::L_U8;  break;
                    case OIIO::TypeDesc::UINT16: out = ftk::ImageType::L_U16; break;
                    case OIIO::TypeDesc::UINT32: out = ftk::ImageType::L_U32; break;
                    case OIIO::TypeDesc::HALF:   out = ftk::ImageType::L_F16; break;
                    case OIIO::TypeDesc::FLOAT:  out = ftk::ImageType::L_F32; break;
                    default: break;
                    }
                }
                else if (2 == oiio.nchannels)
                {
                    switch (oiio.format.basetype)
                    {
                    case OIIO::TypeDesc::UINT8:  out = ftk::ImageType::LA_U8;  break;
                    case OIIO::TypeDesc::UINT16: out = ftk::ImageType::LA_U16; break;
                    case OIIO::TypeDesc::UINT32: out = ftk::ImageType::LA_U32; break;
                    case OIIO::TypeDesc::HALF:   out = ftk::ImageType::LA_F16; break;
                    case OIIO::TypeDesc::FLOAT:  out = ftk::ImageType::LA_F32; break;
                    default: break;
                    }
                }
                else if (3 == oiio.nchannels)
                {
                    switch (oiio.format.basetype)
                    {
                    case OIIO::TypeDesc::UINT8:  out = ftk::ImageType::RGB_U8;  break;
                    case OIIO::TypeDesc::UINT16: out = ftk::ImageType::RGB_U16; break;
                    case OIIO::TypeDesc::UINT32: out = ftk::ImageType::RGB_U32; break;
                    case OIIO::TypeDesc::HALF:   out = ftk::ImageType::RGB_F16; break;
                    case OIIO::TypeDesc::FLOAT:  out = ftk::ImageType::RGB_F32; break;
                    default: break;
                    }
                }
                else if (oiio.nchannels >= 4)
                {
                    switch (oiio.format.basetype)
                    {
                    case OIIO::TypeDesc::UINT8:  out = ftk::ImageType::RGBA_U8;  break;
                    case OIIO::TypeDesc::UINT16: out = ftk::ImageType::RGBA_U16; break;
                    case OIIO::TypeDesc::UINT32: out = ftk::ImageType::RGBA_U32; break;
                    case OIIO::TypeDesc::HALF:   out = ftk::ImageType::RGBA_F16; break;
                    case OIIO::TypeDesc::FLOAT:  out = ftk::ImageType::RGBA_F32; break;
                    default: break;
                    }
                }
                return out;
            }
        }

        void ImageFileNode::exec(
            const std::shared_ptr<ftk::IRender>& render,
            const OTIO_NS::RationalTime& time)
        {
            INode::exec(render, time);
            FTK_P();
            const std::string path = _attr->getItem("Path");
            if (!path.empty() && (!p.image || path != p.path))
            {
                p.path = path;
                p.image.reset();
                _outputs[0].reset();
                try
                {
                    const auto oiioInput = OIIO::ImageInput::open(path);
                    if (!oiioInput)
                    {
                        throw std::runtime_error(OIIO::geterror());
                    }
                    const auto oiioSpec = oiioInput->spec();
                    const ftk::ImageType imageType = fromOIIO(oiioSpec);
                    if (ftk::ImageType::None == imageType)
                    {
                        std::stringstream ss;
                        ss << "Unsupported file: " << path;
                        throw std::runtime_error(ss.str());
                    }
                    ftk::ImageInfo imageInfo(oiioSpec.width, oiioSpec.height, imageType);
                    p.image = ftk::Image::create(imageInfo);
                    if (!oiioInput->read_image(
                        0,
                        0,
                        0,
                        ftk::getChannelCount(imageType),
                        oiioSpec.format,
                        p.image->getData()))
                    {
                        throw std::runtime_error(OIIO::geterror());
                    }
                }
                catch (const std::exception&)
                {
                    //! \todo
                }
            }
            if (p.image)
            {
                const ftk::Size2I size = p.image->getSize();
                if (size.isValid())
                {
                    ftk::gl::OffscreenBufferOptions offscreenBufferOptions;
                    offscreenBufferOptions.color = ftk::ImageType::RGBA_F32;
                    if (ftk::gl::doCreate(_outputs[0], size, offscreenBufferOptions))
                    {
                        _outputs[0] = ftk::gl::OffscreenBuffer::create(size, offscreenBufferOptions);
                    }
                    ftk::gl::OffscreenBufferBinding binding(_outputs[0]);
                    const ftk::Size2I size = p.image->getSize();
                    const ftk::Box2I g(0, 0, size.w, size.h);
                    render->setRenderSize(size);
                    render->setViewport(g);
                    render->clearViewport(ftk::Color4F(0.F, 0.F, 0.F, 0.F));
                    const auto pm = ftk::ortho(
                        0.F,
                        static_cast<float>(size.w),
                        static_cast<float>(size.h),
                        0.F,
                        -1.F,
                        1.F);
                    render->setTransform(pm);
                    ftk::ImageOptions imageOptions;
                    imageOptions.cache = false;
                    render->drawImage(p.image, g, ftk::Color4F(1.F, 1.F, 1.F), imageOptions);
                }
            }
        }

        struct ImageSequenceNode::Private
        {
            ftk::Path path;
            OTIO_NS::RationalTime time = invalidTime;
            std::shared_ptr<ftk::Image> image;
        };

        void ImageSequenceNode::_init(const std::shared_ptr<ftk::Context>& context)
        {
            NodeAttr attr;
            attr["Path"] = "";
            attr["StartFrame"] = 0;
            attr["EndFrame"] = 0;
            attr["Loop"] = InputLoop::None;
            INode::_init(context, getNodeInfo(), 0, 1, attr);
            FTK_P();
        }

        ImageSequenceNode::ImageSequenceNode() :
            _p(new Private)
        {}

        ImageSequenceNode::~ImageSequenceNode()
        {}

        NodeInfo ImageSequenceNode::getNodeInfo()
        {
            return { "ImageSequence", "Image Sequence", "Input", "ImageSequenceNode" };
        }

        std::shared_ptr<INode> ImageSequenceNode::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            std::shared_ptr<ImageSequenceNode> out(new ImageSequenceNode);
            out->_init(context);
            return out;
        }

        void ImageSequenceNode::exec(
            const std::shared_ptr<ftk::IRender>& render,
            const OTIO_NS::RationalTime& time)
        {
            INode::exec(render, time);
            FTK_P();

            const ftk::Path path(_attr->getItem("Path"));
            const int startFrame = _attr->getItem("StartFrame");
            const int endFrame = _attr->getItem("EndFrame");
            const InputLoop loop = _attr->getItem("Loop");

            const OTIO_NS::TimeRange timeRange(
                OTIO_NS::RationalTime(startFrame, time.rate()),
                OTIO_NS::RationalTime(endFrame - startFrame + 1, time.rate()));
            const OTIO_NS::RationalTime time2 = getInputLoop(loop, time, timeRange);

            if (!path.isEmpty() && (!p.image || path != p.path || time2 != p.time))
            {
                p.path = path;
                p.time = time2;
                p.image.reset();
                _outputs[0].reset();

                if (timeRange.contains(time2))
                {
                    try
                    {
                        const std::string fileName = path.getFrame(time2.value(), true);
                        const auto oiioInput = OIIO::ImageInput::open(fileName);
                        if (!oiioInput)
                        {
                            throw std::runtime_error(OIIO::geterror());
                        }
                        const auto oiioSpec = oiioInput->spec();
                        const ftk::ImageType imageType = fromOIIO(oiioSpec);
                        if (ftk::ImageType::None == imageType)
                        {
                            std::stringstream ss;
                            ss << "Unsupported file: " << fileName;
                            throw std::runtime_error(ss.str());
                        }
                        ftk::ImageInfo imageInfo(oiioSpec.width, oiioSpec.height, imageType);
                        p.image = ftk::Image::create(imageInfo);
                        if (!oiioInput->read_image(
                            0,
                            0,
                            0,
                            ftk::getChannelCount(imageType),
                            oiioSpec.format,
                            p.image->getData()))
                        {
                            throw std::runtime_error(OIIO::geterror());
                        }
                    }
                    catch (const std::exception&)
                    {
                        //! \todo
                    }
                }
            }
            if (p.image)
            {
                const ftk::Size2I size = p.image->getSize();
                if (size.isValid())
                {
                    ftk::gl::OffscreenBufferOptions offscreenBufferOptions;
                    offscreenBufferOptions.color = ftk::ImageType::RGBA_F32;
                    if (ftk::gl::doCreate(_outputs[0], size, offscreenBufferOptions))
                    {
                        _outputs[0] = ftk::gl::OffscreenBuffer::create(size, offscreenBufferOptions);
                    }
                    ftk::gl::OffscreenBufferBinding binding(_outputs[0]);
                    const ftk::Size2I size = p.image->getSize();
                    const ftk::Box2I g(0, 0, size.w, size.h);
                    render->setRenderSize(size);
                    render->setViewport(g);
                    render->clearViewport(ftk::Color4F(0.F, 0.F, 0.F, 0.F));
                    const auto pm = ftk::ortho(
                        0.F,
                        static_cast<float>(size.w),
                        static_cast<float>(size.h),
                        0.F,
                        -1.F,
                        1.F);
                    render->setTransform(pm);
                    ftk::ImageOptions imageOptions;
                    imageOptions.cache = false;
                    render->drawImage(p.image, g, ftk::Color4F(1.F, 1.F, 1.F), imageOptions);
                }
            }
        }

        struct SVGFileNode::Private
        {
        };

        void SVGFileNode::_init(const std::shared_ptr<ftk::Context>& context)
        {
            NodeAttr attr;
            INode::_init(context, getNodeInfo(), 0, 1, attr);
            FTK_P();
        }

        SVGFileNode::SVGFileNode() :
            _p(new Private)
        {}

        SVGFileNode::~SVGFileNode()
        {}

        NodeInfo SVGFileNode::getNodeInfo()
        {
            return { "SVGFile", "SVG File", "Input", "SVGFileNode" };
        }

        std::shared_ptr<INode> SVGFileNode::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            std::shared_ptr<SVGFileNode> out(new SVGFileNode);
            out->_init(context);
            return out;
        }

        void SVGFileNode::exec(
            const std::shared_ptr<ftk::IRender>& render,
            const OTIO_NS::RationalTime& time)
        {
            INode::exec(render, time);
            FTK_P();
        }
    }
}
