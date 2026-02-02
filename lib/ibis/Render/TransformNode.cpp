// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "TransformNode.h"

#include <ftk/GL/GL.h>
#include <ftk/GL/Mesh.h>
#include <ftk/GL/OffscreenBuffer.h>
#include <ftk/GL/Shader.h>
#include <ftk/Core/IRender.h>

namespace ibis
{
    namespace render
    {
        struct ResizeNode::Private
        {};

        void ResizeNode::_init(const std::shared_ptr<ftk::Context>& context)
        {
            NodeAttr attr;
            attr["Width"] = 1920;
            attr["Height"] = 1080;
            INode::_init(context, getClassNodeInfo(), 1, 1, attr);
        }

        ResizeNode::ResizeNode() :
            _p(new Private)
        {}

        ResizeNode::~ResizeNode()
        {}

        NodeInfo ResizeNode::getClassNodeInfo()
        {
            return { "Resize", "Resize", "Transform" };
        }

        std::shared_ptr<INode> ResizeNode::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            std::shared_ptr<ResizeNode> out(new ResizeNode);
            out->_init(context);
            return out;
        }

        void ResizeNode::exec(
            const std::shared_ptr<ftk::IRender>& render,
            const OTIO_NS::RationalTime& time)
        {
            INode::exec(render, time);
            FTK_P();

            ftk::Size2I size;
            if (_inputs->getItem(0).node)
            {
                const auto& input0 = _inputs->getItem(0).node->getOutputs();
                if (!input0.empty() && input0.front())
                {
                    size.w = _attr->getItem("Width");
                    size.h = _attr->getItem("Height");
                }
                if (size.isValid())
                {
                    if (ftk::gl::doCreate(_outputs[0], size, ftk::ImageType::RGBA_F32))
                    {
                        _outputs[0] = ftk::gl::OffscreenBuffer::create(size, ftk::ImageType::RGBA_F32);
                    }
                    ftk::gl::OffscreenBufferBinding binding(_outputs[0]);
                    render->setRenderSize(size);
                    const ftk::Box2I vp(0, 0, size.w, size.h);
                    render->setViewport(vp);
                    render->clearViewport(ftk::Color4F(0.F, 0.F, 0.F, 0.F));
                    render->setTransform(_getProjection(size));
                    render->drawTexture(input0.front()->getColorID(), vp, true);
                }
            }
            if (!_inputs->getItem(0).node || !size.isValid())
            {
                _outputs[0].reset();
            }

            _imageInfo->setItemOnlyIfChanged(
                0,
                _outputs[0] ?
                ftk::ImageInfo(_outputs[0]->getSize(), _outputs[0]->getType()) :
                ftk::ImageInfo());
        }

        struct CropNode::Private
        {};

        void CropNode::_init(const std::shared_ptr<ftk::Context>& context)
        {
            NodeAttr attr;
            attr["X"] = 200;
            attr["Y"] = 200;
            attr["Width"] = 400;
            attr["Height"] = 400;
            INode::_init(context, getClassNodeInfo(), 1, 1, attr);
        }

        CropNode::CropNode() :
            _p(new Private)
        {}

        CropNode::~CropNode()
        {}

        NodeInfo CropNode::getClassNodeInfo()
        {
            return { "Crop", "Crop", "Transform" };
        }

        std::shared_ptr<INode> CropNode::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            std::shared_ptr<CropNode> out(new CropNode);
            out->_init(context);
            return out;
        }

        void CropNode::exec(
            const std::shared_ptr<ftk::IRender>& render,
            const OTIO_NS::RationalTime& time)
        {
            INode::exec(render, time);
            FTK_P();

            ftk::Size2I size;
            if (_inputs->getItem(0).node)
            {
                const auto& input0 = _inputs->getItem(0).node->getOutputs();
                ftk::Size2I inputSize;
                if (!input0.empty() && input0.front())
                {
                    size.w = _attr->getItem("Width");
                    size.h = _attr->getItem("Height");
                    inputSize = input0.front()->getSize();
                }
                if (size.isValid())
                {
                    if (ftk::gl::doCreate(_outputs[0], size, ftk::ImageType::RGBA_F32))
                    {
                        _outputs[0] = ftk::gl::OffscreenBuffer::create(size, ftk::ImageType::RGBA_F32);
                    }
                    ftk::gl::OffscreenBufferBinding binding(_outputs[0]);
                    render->setRenderSize(size);
                    render->setViewport(ftk::Box2I(0, 0, size.w, size.h));
                    render->clearViewport(ftk::Color4F(0.F, 0.F, 0.F, 0.F));
                    render->setTransform(_getProjection(size));

                    const ftk::Box2I rect(
                        -ftk::V2I(
                            _attr->getItem("X"),
                            inputSize.h - 1 - _attr->getItem("Y") - size.h),
                        inputSize);
                    render->drawTexture(input0.front()->getColorID(), rect, true);
                }
            }
            if (!_inputs->getItem(0).node || !size.isValid())
            {
                _outputs[0].reset();
            }

            _imageInfo->setItemOnlyIfChanged(
                0,
                _outputs[0] ?
                ftk::ImageInfo(_outputs[0]->getSize(), _outputs[0]->getType()) :
                ftk::ImageInfo());
        }
    }
}
