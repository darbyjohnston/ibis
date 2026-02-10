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
        struct ImageInputNode::Private
        {
            std::string path;
            std::shared_ptr<ftk::Image> image;

            std::shared_ptr<ftk::ObservableList<std::string> > subImageNames;
            std::shared_ptr<ftk::ObservableList<std::string> > channelNames;
        };

        void ImageInputNode::_init(
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

        ImageInputNode::ImageInputNode() :
            _p(new Private)
        {}

        ImageInputNode::~ImageInputNode()
        {}

        NodeInfo ImageInputNode::getClassNodeInfo()
        {
            return { "ImageInput", "Image Input", "I/O" };
        }

        std::vector<std::string> ImageInputNode::getExts()
        {
            return std::vector<std::string>({ ".png" });
        }

        std::shared_ptr<INode> ImageInputNode::create(
            const std::shared_ptr<ftk::Context>& context,
            const nlohmann::json& json)
        {
            std::shared_ptr<ImageInputNode> out(new ImageInputNode);
            out->_init(context, json);
            return out;
        }

        std::shared_ptr<ftk::IObservableList<std::string> > ImageInputNode::observeSubImages() const
        {
            return _p->subImageNames;
        }

        std::shared_ptr<ftk::IObservableList<std::string> > ImageInputNode::observeChannels() const
        {
            return _p->channelNames;
        }

        bool ImageInputNode::setAttr(const NodeAttr& value)
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

        void ImageInputNode::exec(
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
                    catch (const std::exception& e)
                    {
                        auto logSystem = _context.lock()->getLogSystem();
                        logSystem->print("ibis::render::ImageInputNode", e.what(), ftk::LogType::Error);
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

        struct SequenceInputNode::Private
        {
            std::string path;
            OTIO_NS::RationalTime time = invalidTime;
            std::shared_ptr<ftk::Image> image;

            std::shared_ptr<ftk::ObservableList<std::string> > subImageNames;
            std::shared_ptr<ftk::ObservableList<std::string> > channelNames;
        };

        void SequenceInputNode::_init(
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

        SequenceInputNode::SequenceInputNode() :
            _p(new Private)
        {}

        SequenceInputNode::~SequenceInputNode()
        {}

        NodeInfo SequenceInputNode::getClassNodeInfo()
        {
            return { "SequenceInput", "Sequence Input", "I/O" };
        }

        std::vector<std::string> SequenceInputNode::getExts()
        {
            return ImageInputNode::getExts();
        }

        std::shared_ptr<INode> SequenceInputNode::create(
            const std::shared_ptr<ftk::Context>& context,
            const nlohmann::json& json)
        {
            std::shared_ptr<SequenceInputNode> out(new SequenceInputNode);
            out->_init(context, json);
            return out;
        }

        std::shared_ptr<ftk::IObservableList<std::string> > SequenceInputNode::observeSubImages() const
        {
            return _p->subImageNames;
        }

        std::shared_ptr<ftk::IObservableList<std::string> > SequenceInputNode::observeChannels() const
        {
            return _p->channelNames;
        }

        bool SequenceInputNode::setAttr(const NodeAttr& value)
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

        void SequenceInputNode::exec(
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
                    auto ioSystem = _context.lock()->getSystem<ftk::ImageIO>();
                    const std::string fileName = ftk::Path(p.path).getFrame(time2.value(), true);
                    if (auto read = ioSystem->read(std::filesystem::u8path(fileName)))
                    {
                        p.image = read->read();
                    }
                }
                catch (const std::exception& e)
                {
                    auto logSystem = _context.lock()->getLogSystem();
                    logSystem->print("ibis::render::SequenceInputNode", e.what(), ftk::LogType::Error);
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
