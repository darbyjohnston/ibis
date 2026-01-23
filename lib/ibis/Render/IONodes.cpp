// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "IONodes.h"

namespace ibis
{
    namespace render
    {
        struct InputNode::Private
        {
        };

        void InputNode::_init(const std::shared_ptr<ftk::Context>& context)
        {
            INode::_init(context, "Input", 0);
            FTK_P();
        }

        InputNode::InputNode() :
            _p(new Private)
        {}

        InputNode::~InputNode()
        {}

        std::string InputNode::getNodeName()
        {
            return "Input";
        }

        std::shared_ptr<INode> InputNode::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            std::shared_ptr<InputNode> out(new InputNode);
            out->_init(context);
            return out;
        }

        struct OutputNode::Private
        {
        };

        void OutputNode::_init(const std::shared_ptr<ftk::Context>&context)
        {
            INode::_init(context, "Output", 1, 0);
            FTK_P();
        }

        OutputNode::OutputNode() :
            _p(new Private)
        {}

        OutputNode::~OutputNode()
        {}

        std::string OutputNode::getNodeName()
        {
            return "Output";
        }

        std::shared_ptr<INode> OutputNode::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            std::shared_ptr<OutputNode> out(new OutputNode);
            out->_init(context);
            return out;
        }
    }
}
