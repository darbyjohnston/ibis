// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "InputNode.h"

#include <ibis/Core/Time.h>

#include <ftk/GL/GL.h>
#include <ftk/Core/Error.h>
#include <ftk/Core/Format.h>
#include <ftk/Core/IRender.h>
#include <ftk/Core/ImageIO.h>
#include <ftk/Core/Path.h>
#include <ftk/Core/String.h>

namespace ibis
{
    namespace render
    {
        struct ImageFileNode::Private
        {
            std::string path;
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
            INode::_init(context, getClassNodeInfo(), 0, 1, attr, json);
            FTK_P();

            p.path = _attr->getItem("Path");

            p.subImageNames = ftk::ObservableList<std::string>::create();
            p.channelNames = ftk::ObservableList<std::string>::create();
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
                        auto ioSystem = _context.lock()->getSystem<ftk::ImageIO>();
                        if (auto read = ioSystem->read(std::filesystem::u8path(p.path)))
                        {
                            p.image = read->read();
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

        struct ImageSequenceNode::Private
        {
            std::string path;
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
            attr["StartFrame"] = 0;
            attr["EndFrame"] = 0;
            attr["Loop"] = InputLoop::None;
            INode::_init(context, getClassNodeInfo(), 0, 1, attr, json);
            FTK_P();

            p.path = _attr->getItem("Path");

            p.subImageNames = ftk::ObservableList<std::string>::create();
            p.channelNames = ftk::ObservableList<std::string>::create();
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
                p.time = invalidTime;
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

            if (!p.path.empty() && timeRange.contains(time2))
            {
                if (!p.image || time2 != p.time)
                {
                    p.time = time2;
                    try
                    {
                        auto ioSystem = _context.lock()->getSystem<ftk::ImageIO>();
                        const std::string fileName = ftk::Path(p.path).getFrame(time2.value(), true);
                        if (auto read = ioSystem->read(std::filesystem::u8path(fileName)))
                        {
                            p.image = read->read();
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
    }
}
