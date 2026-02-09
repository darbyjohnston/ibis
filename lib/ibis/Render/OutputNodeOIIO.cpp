// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "OutputNode.h"

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
        struct ImageOutputNode::Private
        {
        };

        void ImageOutputNode::_init(
            const std::shared_ptr<ftk::Context>& context,
            const nlohmann::json& json)
        {
            NodeAttr attr;
            attr["Path"] = "";
            INode::_init(context, getClassNodeInfo(), 1, 1, attr, json);
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

        void ImageOutputNode::exec(
            const std::shared_ptr<ftk::IRender>& render,
            const OTIO_NS::RationalTime& time)
        {
            INode::exec(render, time);
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
            const std::string path = _attr->getItem("Path");
            if (!path.empty())
            {
                try
                {

                }
                catch (const std::exception& e)
                {

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
