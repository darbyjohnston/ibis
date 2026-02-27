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

#include <future>

namespace ibis
{
    namespace render
    {
        struct USDInputNode::Private
        {
            std::string path;
            OTIO_NS::RationalTime time = invalidTime;
            std::shared_ptr<ftk::Image> image;
        };

        void USDInputNode::_init(
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

            if (!p.path.empty())
            {
                /*try
                {
                    const auto oiioInput = OIIO::ImageInput::open(p.path);
                    if (!oiioInput)
                    {
                        throw std::runtime_error(OIIO::geterror());
                    }
                    p.subImages = getSubImages(oiioInput.get());
                }
                catch (const std::exception& e)
                {
                    auto logSystem = _context.lock()->getLogSystem();
                    logSystem->print("ibis::render::SequenceInputNode", e.what(), ftk::LogType::Error);
                }*/
            }
        }

        USDInputNode::USDInputNode() :
            _p(new Private)
        {}

        USDInputNode::~USDInputNode()
        {}

        NodeInfo USDInputNode::getClassNodeInfo()
        {
            return { "USDInput", "USD Input", "I/O" };
        }

        std::vector<std::string> USDInputNode::getExts()
        {
            return { ".usd", ".usda", ".usdc", ".usdz" };
        }

        std::shared_ptr<INode> USDInputNode::create(
            const std::shared_ptr<ftk::Context>& context,
            const nlohmann::json& json)
        {
            std::shared_ptr<USDInputNode> out(new USDInputNode);
            out->_init(context, json);
            return out;
        }

        bool USDInputNode::setAttr(const NodeAttr& value)
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

                if (!p.path.empty())
                {
                    /*try
                    {
                        const auto oiioInput = OIIO::ImageInput::open(p.path);
                        if (!oiioInput)
                        {
                            throw std::runtime_error(OIIO::geterror());
                        }
                        p.subImages = getSubImages(oiioInput.get());
                    }
                    catch (const std::exception& e)
                    {
                        auto logSystem = _context.lock()->getLogSystem();
                        logSystem->print("ibis::render::SequenceInputNode", e.what(), ftk::LogType::Error);
                    }*/
                }
            }
            return INode::setAttr(tmp);
        }

        void USDInputNode::execInit(const OTIO_NS::RationalTime& time)
        {
            INode::execInit(time);
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
                //const std::string fileName = ftk::Path(p.path).getFrame(time2.value(), true);
                //p.future = std::async(&load, fileName, p.subImage, p.channelGroup);
            }
        }

        void USDInputNode::exec(
            const std::shared_ptr<ftk::IRender>& render,
            const OTIO_NS::RationalTime& time)
        {
            INode::exec(render, time);
            FTK_P();

            /*if (p.future.valid())
            {
                const auto result = p.future.get();
                if (!result.first)
                {
                    auto logSystem = _context.lock()->getLogSystem();
                    logSystem->print("ibis::render::SequenceInputNode", result.second, ftk::LogType::Error);
                }
                else
                {
                    p.image = result.first;
                }
            }*/

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
