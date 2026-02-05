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

#include <lunasvg/lunasvg.h>

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

        struct SVGFileNode::Private
        {
            std::string path;
            std::shared_ptr<ftk::Image> image;
        };

        void SVGFileNode::_init(
            const std::shared_ptr<ftk::Context>& context,
            const nlohmann::json& json)
        {
            NodeAttr attr;
            attr["Path"] = "";
            INode::_init(context, getClassNodeInfo(), 0, 1, attr, json);
        }

        SVGFileNode::SVGFileNode() :
            _p(new Private)
        {}

        SVGFileNode::~SVGFileNode()
        {}

        NodeInfo SVGFileNode::getClassNodeInfo()
        {
            return { "SVGFile", "SVG File", "Input" };
        }

        std::shared_ptr<INode> SVGFileNode::create(
            const std::shared_ptr<ftk::Context>& context,
            const nlohmann::json& json)
        {
            std::shared_ptr<SVGFileNode> out(new SVGFileNode);
            out->_init(context, json);
            return out;
        }

        void SVGFileNode::exec(
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
                    auto svg = lunasvg::Document::loadFromFile(path);
                    const int w = svg->width();
                    const int h = svg->height();
                    auto bitmap = svg->renderToBitmap(w, h, 0x00000000);
                    if (!bitmap.isNull())
                    {
                        ftk::ImageInfo imageInfo(w, h, ftk::ImageType::RGBA_U8);
                        imageInfo.layout.mirror.y = true;
                        p.image = ftk::Image::create(imageInfo);
                        for (int y = 0; y < h; ++y)
                        {
                            uint8_t* imageP = reinterpret_cast<uint8_t*>(p.image->getData()) + y * w * 4;
                            const uint8_t* bitmapP = bitmap.data() + y * w * 4;
                            for (int x = 0; x < w; ++x, imageP += 4, bitmapP += 4)
                            {
                                imageP[0] = bitmapP[2];
                                imageP[1] = bitmapP[1];
                                imageP[2] = bitmapP[0];
                                imageP[3] = bitmapP[3];
                            }
                        }
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
                    const ftk::Size2I size = p.image->getSize();
                    const ftk::Box2I g(0, 0, size.w, size.h);
                    render->setRenderSize(size);
                    render->setViewport(g);
                    render->clearViewport(ftk::Color4F(0.F, 0.F, 0.F, 0.F));
                    render->setTransform(_getProjection(size));
                    ftk::ImageOptions imageOptions;
                    imageOptions.cache = false;
                    render->drawImage(p.image, g, ftk::Color4F(1.F, 1.F, 1.F), imageOptions);
                }
            }
            _outputInfo->setItemOnlyIfChanged(
                0,
                _outputs[0] ?
                ftk::gl::TextureInfo(_outputs[0]->getSize(), _outputs[0]->getType()) :
                ftk::gl::TextureInfo());
        }
    }
}
