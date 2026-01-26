// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "InputNode.h"

#include <ftk/GL/GL.h>
#include <ftk/GL/OffscreenBuffer.h>
#include <ftk/Core/IRender.h>

#include <OpenImageIO/filesystem.h>
#include <OpenImageIO/imagebufalgo.h>

namespace ibis
{
    namespace render
    {
        struct ImageFileNode::Private
        {
            std::string path;
            std::shared_ptr<ftk::Image> image;
        };

        void ImageFileNode::_init(const std::shared_ptr<ftk::Context>& context)
        {
            NodeAttr attr;
            attr["Path"] = "";
            INode::_init(context, getNodeID(), 0, 1, attr);
            FTK_P();
        }

        ImageFileNode::ImageFileNode() :
            _p(new Private)
        {}

        ImageFileNode::~ImageFileNode()
        {}

        std::string ImageFileNode::getNodeID()
        {
            return "Image File";
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
                        try
                        {
                            _outputs[0] = ftk::gl::OffscreenBuffer::create(size, offscreenBufferOptions);
                        }
                        catch (const std::exception&)
                        {
                            //! \todo
                        }
                    }
                }
            }
            if (_outputs[0] && p.image)
            {
                const ftk::Size2I size = p.image->getSize();
                const ftk::Box2I g(0, 0, size.w, size.h);
                ftk::gl::OffscreenBufferBinding binding(_outputs[0]);
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
                render->drawImage(p.image, g);
            }
        }

        struct ImageFileSequenceNode::Private
        {
        };

        void ImageFileSequenceNode::_init(const std::shared_ptr<ftk::Context>& context)
        {
            NodeAttr attr;
            INode::_init(context, getNodeID(), 0, 1, attr);
            FTK_P();
        }

        ImageFileSequenceNode::ImageFileSequenceNode() :
            _p(new Private)
        {}

        ImageFileSequenceNode::~ImageFileSequenceNode()
        {}

        std::string ImageFileSequenceNode::getNodeID()
        {
            return "Image File Sequence";
        }

        std::shared_ptr<INode> ImageFileSequenceNode::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            std::shared_ptr<ImageFileSequenceNode> out(new ImageFileSequenceNode);
            out->_init(context);
            return out;
        }

        void ImageFileSequenceNode::exec(
            const std::shared_ptr<ftk::IRender>& render,
            const OTIO_NS::RationalTime& time)
        {
            INode::exec(render, time);
            FTK_P();
        }

        struct SVGFileNode::Private
        {
        };

        void SVGFileNode::_init(const std::shared_ptr<ftk::Context>& context)
        {
            NodeAttr attr;
            INode::_init(context, getNodeID(), 0, 1, attr);
            FTK_P();
        }

        SVGFileNode::SVGFileNode() :
            _p(new Private)
        {}

        SVGFileNode::~SVGFileNode()
        {}

        std::string SVGFileNode::getNodeID()
        {
            return "Image File Sequence";
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
