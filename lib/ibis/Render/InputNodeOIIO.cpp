// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "InputNode.h"

#include <ibis/Core/Time.h>

#include <ftk/GL/GL.h>
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
        namespace
        {
            struct ChannelGroup
            {
                std::string name;
                ftk::Size2I size;
                ftk::ImageType type = ftk::ImageType::None;
                OIIO::TypeDesc oiioFormat;
                int start = 0;
            };

            struct SubImage
            {
                std::vector<ChannelGroup> channels;
            };

            ftk::ImageType fromOIIO(const OIIO::TypeDesc& oiioType, int channelCount)
            {
                ftk::ImageType out = ftk::ImageType::None;
                if (1 == channelCount)
                {
                    switch (oiioType.basetype)
                    {
                    case OIIO::TypeDesc::UINT8:  out = ftk::ImageType::L_U8;  break;
                    case OIIO::TypeDesc::UINT16: out = ftk::ImageType::L_U16; break;
                    case OIIO::TypeDesc::UINT32: out = ftk::ImageType::L_U32; break;
                    case OIIO::TypeDesc::HALF:   out = ftk::ImageType::L_F16; break;
                    case OIIO::TypeDesc::FLOAT:  out = ftk::ImageType::L_F32; break;
                    default: break;
                    }
                }
                else if (2 == channelCount)
                {
                    switch (oiioType.basetype)
                    {
                    case OIIO::TypeDesc::UINT8:  out = ftk::ImageType::LA_U8;  break;
                    case OIIO::TypeDesc::UINT16: out = ftk::ImageType::LA_U16; break;
                    case OIIO::TypeDesc::UINT32: out = ftk::ImageType::LA_U32; break;
                    case OIIO::TypeDesc::HALF:   out = ftk::ImageType::LA_F16; break;
                    case OIIO::TypeDesc::FLOAT:  out = ftk::ImageType::LA_F32; break;
                    default: break;
                    }
                }
                else if (3 == channelCount)
                {
                    switch (oiioType.basetype)
                    {
                    case OIIO::TypeDesc::UINT8:  out = ftk::ImageType::RGB_U8;  break;
                    case OIIO::TypeDesc::UINT16: out = ftk::ImageType::RGB_U16; break;
                    case OIIO::TypeDesc::UINT32: out = ftk::ImageType::RGB_U32; break;
                    case OIIO::TypeDesc::HALF:   out = ftk::ImageType::RGB_F16; break;
                    case OIIO::TypeDesc::FLOAT:  out = ftk::ImageType::RGB_F32; break;
                    default: break;
                    }
                }
                else if (channelCount >= 4)
                {
                    switch (oiioType.basetype)
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

            bool compare(const std::string& a, const std::string& b)
            {
                bool out = false;
                if (a.size() > 2 &&
                    b.size() > 2 &&
                    '.' == a[a.size() - 2] &&
                    '.' == b[b.size() - 2] &&
                    a.substr(0, a.size() - 2) == b.substr(0, b.size() - 2))
                {
                    out = true;
                }
                else if (1 == a.size() && 1 == b.size())
                {
                    out = true;
                }
                return out;
            }

            std::string getName(const std::vector<std::string>& value)
            {
                std::string out;
                std::vector<std::string> prefixes;
                std::vector<std::string> channels;
                for (const auto& i : value)
                {
                    if (i.size() > 2 && '.' == i[i.size() - 2])
                    {
                        prefixes.push_back(i.substr(0, i.size() - 2));
                        channels.push_back(i.substr(i.size() - 1, 1));
                    }
                    else
                    {
                        channels.push_back(i);
                    }
                }
                if (!prefixes.empty())
                {
                    out = prefixes.front();
                }
                if (!channels.empty())
                {
                    if (!out.empty())
                    {
                        out += ".";
                    }
                    out += ftk::join(channels, "");
                }
                return out;
            }

            bool parse(const OIIO::ImageSpec& oiioSpec, int& c, ChannelGroup& group)
            {
                bool out = false;
                if (c < static_cast<int>(oiioSpec.channelnames.size()) - 3 &&
                    compare(oiioSpec.channel_name(c), oiioSpec.channel_name(c + 1)) &&
                    compare(oiioSpec.channel_name(c), oiioSpec.channel_name(c + 2)) &&
                    compare(oiioSpec.channel_name(c), oiioSpec.channel_name(c + 3)) &&
                    oiioSpec.channelformat(c + 1) == oiioSpec.channelformat(c) &&
                    oiioSpec.channelformat(c + 2) == oiioSpec.channelformat(c) &&
                    oiioSpec.channelformat(c + 3) == oiioSpec.channelformat(c) &&
                    fromOIIO(oiioSpec.channelformat(c), 4) != ftk::ImageType::None)
                {
                    group.name = getName(
                        {
                            oiioSpec.channel_name(c),
                            oiioSpec.channel_name(c + 1),
                            oiioSpec.channel_name(c + 2),
                            oiioSpec.channel_name(c + 3)
                        });
                    group.size.w = oiioSpec.width;
                    group.size.h = oiioSpec.height;
                    group.type = fromOIIO(oiioSpec.channelformat(c), 4);
                    group.oiioFormat = oiioSpec.channelformat(c);
                    group.start = c;
                    c += 4;
                    out = true;
                }
                else if (c < static_cast<int>(oiioSpec.channelnames.size()) - 2 &&
                    compare(oiioSpec.channel_name(c), oiioSpec.channel_name(c + 1)) &&
                    compare(oiioSpec.channel_name(c), oiioSpec.channel_name(c + 2)) &&
                    oiioSpec.channelformat(c + 1) == oiioSpec.channelformat(c) &&
                    oiioSpec.channelformat(c + 2) == oiioSpec.channelformat(c) &&
                    fromOIIO(oiioSpec.channelformat(c), 3) != ftk::ImageType::None)
                {
                    group.name = getName(
                        {
                            oiioSpec.channel_name(c),
                            oiioSpec.channel_name(c + 1),
                            oiioSpec.channel_name(c + 2)
                        });
                    group.size.w = oiioSpec.width;
                    group.size.h = oiioSpec.height;
                    group.type = fromOIIO(oiioSpec.channelformat(c), 3);
                    group.oiioFormat = oiioSpec.channelformat(c);
                    group.start = c;
                    c += 3;
                    out = true;
                }
                else if (fromOIIO(oiioSpec.format, 1) != ftk::ImageType::None)
                {
                    group.name = oiioSpec.channel_name(c);
                    group.size.w = oiioSpec.width;
                    group.size.h = oiioSpec.height;
                    group.type = fromOIIO(oiioSpec.channelformat(c), 1);
                    group.oiioFormat = oiioSpec.channelformat(c);
                    group.start = c;
                    ++c;
                    out = true;
                }
                else
                {
                    ++c;
                }
                return out;
            }

            std::vector<SubImage> getSubImages(OIIO::ImageInput* oiioInput)
            {
                std::vector<SubImage> out;
                for (int sub = 0; oiioInput->seek_subimage(sub, 0); ++sub)
                {
                    const auto& oiioSpec = oiioInput->spec();
                    const ftk::Size2I size(oiioSpec.width, oiioSpec.height);
                    SubImage subImage;
                    int c = 0;
                    while (c < oiioSpec.nchannels)
                    {
                        ChannelGroup group;
                        if (parse(oiioSpec, c, group))
                        {
                            subImage.channels.push_back(group);
                        }
                    }
                    out.push_back(subImage);
                }
                return out;
            }

            std::vector<std::string> getSubImageNames(const std::vector<SubImage>& subImages)
            {
                std::vector<std::string> out;
                for (const auto& subImage : subImages)
                {
                    out.push_back(
                        !subImage.channels.empty() ?
                        subImage.channels.front().name :
                        std::string());
                }
                return out;
            }

            std::vector<std::string> getChannelNames(const std::vector<SubImage>& subImages, int subImage)
            {
                std::vector<std::string> out;
                if (subImage >= 0 && subImage < subImages.size())
                {
                    for (const auto& group : subImages[subImage].channels)
                    {
                        out.push_back(group.name);
                    }
                }
                return out;
            }
        }

        std::vector<std::string> ImageFileNode::getExts()
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

        struct ImageFileNode::Private
        {
            std::string path;
            std::vector<SubImage> subImages;
            int subImage = 0;
            int channelGroup = 0;
            std::shared_ptr<ftk::Image> image;

            std::shared_ptr<ftk::ObservableList<std::string> > subImageNames;
            std::shared_ptr<ftk::ObservableList<std::string> > channelNames;
        };

        void ImageFileNode::_init(
            const std::shared_ptr<ftk::Context>& context,
            const nlohmann::json& json)
        {
            NodeAttr attr;
            attr["Path"] = "";
            attr["SubImage"] = 0;
            attr["ChannelGroup"] = 0;
            INode::_init(context, getClassNodeInfo(), 0, 1, attr, json);
            FTK_P();

            p.path = _attr->getItem("Path");
            p.subImage = _attr->getItem("SubImage");
            p.channelGroup = _attr->getItem("ChannelGroup");

            if (!p.path.empty())
            {
                try
                {
                    const auto oiioInput = OIIO::ImageInput::open(p.path);
                    if (!oiioInput)
                    {
                        throw std::runtime_error(OIIO::geterror());
                    }
                    p.subImages = getSubImages(oiioInput.get());
                }
                catch (const std::exception&)
                {
                    //! \todo
                }
            }

            p.subImageNames = ftk::ObservableList<std::string>::create(
                getSubImageNames(p.subImages));
            p.channelNames = ftk::ObservableList<std::string>::create(
                getChannelNames(p.subImages, p.subImage));
        }

        ImageFileNode::ImageFileNode() :
            _p(new Private)
        {}

        ImageFileNode::~ImageFileNode()
        {}

        NodeInfo ImageFileNode::getClassNodeInfo()
        {
            return { "ImageFile", "Image File", "Input" };
        }

        std::shared_ptr<INode> ImageFileNode::create(
            const std::shared_ptr<ftk::Context>& context,
            const nlohmann::json& json)
        {
            std::shared_ptr<ImageFileNode> out(new ImageFileNode);
            out->_init(context, json);
            return out;
        }

        std::shared_ptr<ftk::IObservableList<std::string> > ImageFileNode::observeSubImages() const
        {
            return _p->subImageNames;
        }

        std::shared_ptr<ftk::IObservableList<std::string> > ImageFileNode::observeChannels() const
        {
            return _p->channelNames;
        }

        bool ImageFileNode::setAttr(const NodeAttr& value)
        {
            FTK_P();
            NodeAttr tmp = value;
            auto i = tmp.find("Path");
            if (i != tmp.end() &&
                i->second.is_string() &&
                static_cast<std::string>(i->second) != p.path)
            {
                p.path = i->second;
                p.subImage = 0;
                p.channelGroup = 0;
                tmp["SubImage"] = 0;
                tmp["ChannelGroup"] = 0;
                p.image.reset();
                _outputs[0].reset();
                p.subImages.clear();

                if (!p.path.empty())
                {
                    try
                    {
                        const auto oiioInput = OIIO::ImageInput::open(p.path);
                        if (!oiioInput)
                        {
                            throw std::runtime_error(OIIO::geterror());
                        }
                        p.subImages = getSubImages(oiioInput.get());
                    }
                    catch (const std::exception&)
                    {
                        //! \todo
                    }
                }
                p.subImageNames->setIfChanged(getSubImageNames(p.subImages));
                p.channelNames->setIfChanged(getChannelNames(p.subImages, p.subImage));
            }
            i = tmp.find("SubImage");
            if (i != tmp.end() &&
                i->second.is_number_integer() &&
                static_cast<int>(i->second) != p.subImage)
            {
                p.subImage = i->second;
                p.channelNames->setIfChanged(getChannelNames(p.subImages, p.subImage));
                p.image.reset();
                _outputs[0].reset();
            }
            i = tmp.find("ChannelGroup");
            if (i != tmp.end() &&
                i->second.is_number_integer() &&
                static_cast<int>(i->second) != p.channelGroup)
            {
                p.channelGroup = i->second;
                p.image.reset();
                _outputs[0].reset();
            }
            return INode::setAttr(tmp);
        }

        void ImageFileNode::exec(
            const std::shared_ptr<ftk::IRender>& render,
            const OTIO_NS::RationalTime& time)
        {
            INode::exec(render, time);
            FTK_P();

            if (!p.path.empty())
            {
                if (!p.image)
                {
                    try
                    {
                        const auto oiioInput = OIIO::ImageInput::open(p.path);
                        if (!oiioInput)
                        {
                            throw std::runtime_error(OIIO::geterror());
                        }
                        auto subImages = getSubImages(oiioInput.get());
                        std::optional<ChannelGroup> group;
                        if (p.subImage >= 0 &&
                            p.subImage < subImages.size() &&
                            p.channelGroup >= 0 &&
                            p.channelGroup < subImages[p.subImage].channels.size())
                        {
                            group = subImages[p.subImage].channels[p.channelGroup];
                        }
                        if (!group.has_value())
                        {
                            std::stringstream ss;
                            ss << "Unsupported file: " << p.path;
                            throw std::runtime_error(ss.str());
                        }

                        ftk::ImageInfo imageInfo(group->size, group->type);
                        imageInfo.layout.mirror.y = true;
                        p.image = ftk::Image::create(imageInfo);
                        if (!oiioInput->read_image(
                            p.subImage,
                            0,
                            group->start,
                            group->start + ftk::getChannelCount(group->type),
                            group->oiioFormat,
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

            ftk::gl::TextureInfo info;
            if (p.image)
            {
                info.size = p.image->getSize();
                info.type = ftk::gl::getTextureType(p.image->getType());
                if (info.isValid())
                {
                    if (ftk::gl::doCreate(_outputs[0], info))
                    {
                        _outputs[0] = ftk::gl::OffscreenBuffer::create(info);
                    }
                    ftk::gl::OffscreenBufferBinding binding(_outputs[0]);
                    const ftk::Box2I g(0, 0, info.size.w, info.size.h);
                    render->setRenderSize(info.size);
                    render->setViewport(g);
                    render->clearViewport(ftk::Color4F(0.F, 0.F, 0.F, 0.F));
                    render->setTransform(_getProjection(info.size));
                    ftk::ImageOptions imageOptions;
                    imageOptions.cache = false;
                    render->drawImage(p.image, g, ftk::Color4F(1.F, 1.F, 1.F), imageOptions);
                }
            }
            _outputInfo->setItemOnlyIfChanged(0, info);
        }

        std::vector<std::string> ImageSequenceNode::getExts()
        {
            return ImageFileNode::getExts();
        }

        struct ImageSequenceNode::Private
        {
            std::string path;
            std::vector<SubImage> subImages;
            int subImage = 0;
            int channelGroup = 0;
            OTIO_NS::RationalTime time = invalidTime;
            std::shared_ptr<ftk::Image> image;

            std::shared_ptr<ftk::ObservableList<std::string> > subImageNames;
            std::shared_ptr<ftk::ObservableList<std::string> > channelNames;
        };

        void ImageSequenceNode::_init(
            const std::shared_ptr<ftk::Context>& context,
            const nlohmann::json& json)
        {
            NodeAttr attr;
            attr["Path"] = "";
            attr["SubImage"] = 0;
            attr["ChannelGroup"] = 0;
            attr["StartFrame"] = 0;
            attr["EndFrame"] = 0;
            attr["Loop"] = InputLoop::None;
            INode::_init(context, getClassNodeInfo(), 0, 1, attr, json);
            FTK_P();

            p.path = _attr->getItem("Path");
            p.subImage = _attr->getItem("SubImage");
            p.channelGroup = _attr->getItem("ChannelGroup");

            if (!p.path.empty())
            {
                try
                {
                    const auto oiioInput = OIIO::ImageInput::open(p.path);
                    if (!oiioInput)
                    {
                        throw std::runtime_error(OIIO::geterror());
                    }
                    p.subImages = getSubImages(oiioInput.get());
                }
                catch (const std::exception&)
                {
                    //! \todo
                }
            }

            p.subImageNames = ftk::ObservableList<std::string>::create(
                getSubImageNames(p.subImages));
            p.channelNames = ftk::ObservableList<std::string>::create(
                getChannelNames(p.subImages, p.subImage));
        }

        ImageSequenceNode::ImageSequenceNode() :
            _p(new Private)
        {}

        ImageSequenceNode::~ImageSequenceNode()
        {}

        NodeInfo ImageSequenceNode::getClassNodeInfo()
        {
            return { "ImageSequence", "Image Sequence", "Input" };
        }

        std::shared_ptr<INode> ImageSequenceNode::create(
            const std::shared_ptr<ftk::Context>& context,
            const nlohmann::json& json)
        {
            std::shared_ptr<ImageSequenceNode> out(new ImageSequenceNode);
            out->_init(context, json);
            return out;
        }

        std::shared_ptr<ftk::IObservableList<std::string> > ImageSequenceNode::observeSubImages() const
        {
            return _p->subImageNames;
        }

        std::shared_ptr<ftk::IObservableList<std::string> > ImageSequenceNode::observeChannels() const
        {
            return _p->channelNames;
        }

        bool ImageSequenceNode::setAttr(const NodeAttr& value)
        {
            FTK_P();
            NodeAttr tmp = value;
            auto i = tmp.find("Path");
            if (i != tmp.end() &&
                i->second.is_string() &&
                static_cast<std::string>(i->second) != p.path)
            {
                p.path = i->second;
                p.subImage = 0;
                p.channelGroup = 0;
                tmp["SubImage"] = 0;
                tmp["ChannelGroup"] = 0;
                p.time = invalidTime;
                p.image.reset();
                _outputs[0].reset();
                p.subImages.clear();

                if (!p.path.empty())
                {
                    try
                    {
                        const auto oiioInput = OIIO::ImageInput::open(p.path);
                        if (!oiioInput)
                        {
                            throw std::runtime_error(OIIO::geterror());
                        }
                        p.subImages = getSubImages(oiioInput.get());
                    }
                    catch (const std::exception&)
                    {
                        //! \todo
                    }
                }
                p.subImageNames->setIfChanged(getSubImageNames(p.subImages));
                p.channelNames->setIfChanged(getChannelNames(p.subImages, p.subImage));
            }
            i = tmp.find("SubImage");
            if (i != tmp.end() &&
                i->second.is_number_integer() &&
                static_cast<int>(i->second) != p.subImage)
            {
                p.subImage = i->second;
                p.channelNames->setIfChanged(getChannelNames(p.subImages, p.subImage));
                p.image.reset();
                _outputs[0].reset();
            }
            i = tmp.find("ChannelGroup");
            if (i != tmp.end() &&
                i->second.is_number_integer() &&
                static_cast<int>(i->second) != p.channelGroup)
            {
                p.channelGroup = i->second;
                p.image.reset();
                _outputs[0].reset();
            }
            return INode::setAttr(tmp);
        }

        void ImageSequenceNode::exec(
            const std::shared_ptr<ftk::IRender>& render,
            const OTIO_NS::RationalTime& time)
        {
            INode::exec(render, time);
            FTK_P();

            const int startFrame = _attr->getItem("StartFrame");
            const int endFrame = _attr->getItem("EndFrame");
            const InputLoop loop = _attr->getItem("Loop");

            const OTIO_NS::TimeRange timeRange(
                OTIO_NS::RationalTime(startFrame, time.rate()),
                OTIO_NS::RationalTime(endFrame - startFrame + 1, time.rate()));
            const OTIO_NS::RationalTime time2 = getInputLoop(
                loop,
                time + timeRange.start_time(),
                timeRange);
            if (time2 != p.time || !timeRange.contains(time2))
            {
                p.time = time2;
                p.image.reset();
                _outputs[0].reset();
            }

            if (!p.path.empty() && !p.image)
            {
                try
                {
                    const std::string fileName = ftk::Path(p.path).getFrame(time2.value(), true);
                    const auto oiioInput = OIIO::ImageInput::open(fileName);
                    if (!oiioInput)
                    {
                        throw std::runtime_error(OIIO::geterror());
                    }
                    auto subImages = getSubImages(oiioInput.get());
                    std::optional<ChannelGroup> group;
                    if (p.subImage >= 0 &&
                        p.subImage < subImages.size() &&
                        p.channelGroup >= 0 &&
                        p.channelGroup < subImages[p.subImage].channels.size())
                    {
                        group = subImages[p.subImage].channels[p.channelGroup];
                    }
                    if (!group.has_value())
                    {
                        std::stringstream ss;
                        ss << "Unsupported file: " << fileName;
                        throw std::runtime_error(ss.str());
                    }

                    ftk::ImageInfo imageInfo(group->size, group->type);
                    imageInfo.layout.mirror.y = true;
                    p.image = ftk::Image::create(imageInfo);
                    if (!oiioInput->read_image(
                        p.subImage,
                        0,
                        group->start,
                        group->start + ftk::getChannelCount(group->type),
                        group->oiioFormat,
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

            ftk::gl::TextureInfo info;
            if (p.image)
            {
                info.size = p.image->getSize();
                info.type = ftk::gl::getTextureType(p.image->getType());
                if (info.isValid())
                {
                    if (ftk::gl::doCreate(_outputs[0], info))
                    {
                        _outputs[0] = ftk::gl::OffscreenBuffer::create(info);
                    }
                    ftk::gl::OffscreenBufferBinding binding(_outputs[0]);
                    const ftk::Box2I g(0, 0, info.size.w, info.size.h);
                    render->setRenderSize(info.size);
                    render->setViewport(g);
                    render->clearViewport(ftk::Color4F(0.F, 0.F, 0.F, 0.F));
                    render->setTransform(_getProjection(info.size));
                    ftk::ImageOptions imageOptions;
                    imageOptions.cache = false;
                    render->drawImage(p.image, g, ftk::Color4F(1.F, 1.F, 1.F), imageOptions);
                }
            }
            _outputInfo->setItemOnlyIfChanged(0, info);
        }
    }
}
