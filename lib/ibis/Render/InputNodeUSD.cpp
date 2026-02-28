// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "InputNode.h"

#include "USDRender.h"

#include <ibis/Core/Time.h>

#include <ftk/GL/GL.h>
#include <ftk/Core/Error.h>
#include <ftk/Core/Format.h>
#include <ftk/Core/IRender.h>
#include <ftk/Core/Path.h>
#include <ftk/Core/String.h>

#include <atomic>

namespace ibis
{
    namespace render
    {
        namespace
        {
            std::atomic<size_t> objectCount = 0;
            std::shared_ptr<usd::Render> render;
        }

        struct USDInputNode::Private
        {
            std::string path;
            OTIO_NS::RationalTime time = invalidTime;
            usd::Info info;
            usd::Request request;
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

            ++objectCount;
            if (1 == objectCount)
            {
                render = usd::Render::create(context->getLogSystem());
            }

            p.path = _attr->getItem("Path");

            if (!p.path.empty())
            {
                p.info = render->getInfo(p.path).future.get();
                if (p.info.timeRange.is_valid_range())
                {
                    attr["StartFrame"] = p.info.timeRange.start_time().value();
                    attr["EndFrame"] = p.info.timeRange.end_time_inclusive().value();
                }
                else
                {
                    auto logSystem = _context.lock()->getLogSystem();
                    logSystem->print(
                        "ibis::render::USDInputNode",
                        ftk::Format("Cannot open: {0}").arg(p.path),
                        ftk::LogType::Error);
                }
            }
        }

        USDInputNode::USDInputNode() :
            _p(new Private)
        {}

        USDInputNode::~USDInputNode()
        {
            FTK_P();
            --objectCount;
            if (0 == objectCount)
            {
                render.reset();
            }
        }

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

                p.info = render->getInfo(p.path).future.get();
                if (p.info.timeRange.is_valid_range())
                {
                    tmp["StartFrame"] = p.info.timeRange.start_time().value();
                    tmp["EndFrame"] = p.info.timeRange.end_time_inclusive().value();
                }
                else
                {
                    auto logSystem = _context.lock()->getLogSystem();
                    logSystem->print(
                        "ibis::render::USDInputNode",
                        ftk::Format("Cannot open: {0}").arg(p.path),
                        ftk::LogType::Error);
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
                p.request = render->render(p.path, time2);
            }
        }

        void USDInputNode::exec(
            const std::shared_ptr<ftk::IRender>& render,
            const OTIO_NS::RationalTime& time)
        {
            INode::exec(render, time);
            FTK_P();

            if (p.request.future.valid())
            {
                p.image = p.request.future.get();
                if (!p.image)
                {
                    auto logSystem = _context.lock()->getLogSystem();
                    logSystem->print(
                        "ibis::render::USDInputNode",
                        ftk::Format("Cannot render {0} time: {1}").arg(p.path).arg(time.value()),
                        ftk::LogType::Error);
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
