// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "GenNode.h"

#include <ftk/GL/GL.h>
#include <ftk/GL/OffscreenBuffer.h>
#include <ftk/Core/IRender.h>
#include <ftk/Core/Color.h>

namespace ibis
{
    namespace render
    {
        struct SolidColorNode::Private
        {
        };

        void SolidColorNode::_init(const std::shared_ptr<ftk::Context>& context)
        {
            NodeAttr attr;
            attr["Size"] = ftk::Size2I(256, 256);
            attr["Color"] = ftk::Color4F(1.F, .7F, 0.F);
            INode::_init(context, getNodeID(), 0, 1, attr);
            FTK_P();
        }

        SolidColorNode::SolidColorNode() :
            _p(new Private)
        {}

        SolidColorNode::~SolidColorNode()
        {}

        std::string SolidColorNode::getNodeID()
        {
            return "SolidColor";
        }

        std::shared_ptr<INode> SolidColorNode::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            std::shared_ptr<SolidColorNode> out(new SolidColorNode);
            out->_init(context);
            return out;
        }

        void SolidColorNode::exec(const std::shared_ptr<ftk::IRender>& render)
        {
            FTK_P();
            ftk::gl::OffscreenBufferOptions offscreenBufferOptions;
            offscreenBufferOptions.color = ftk::ImageType::RGBA_F32;
            const ftk::Size2I size = _attr->getItem("Size");
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

            if (_outputs[0])
            {
                ftk::gl::OffscreenBufferBinding binding(_outputs[0]);
                render->setRenderSize(size);
                render->setViewport(ftk::Box2I(0, 0, size.w, size.h));
                render->clearViewport(_attr->getItem("Color"));
            }
        }
    }
}
