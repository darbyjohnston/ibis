// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "INodeWidget.h"

#include <ibis/Models/Document.h>

#include <ibis/Render/GraphCmd.h>
#include <ibis/Render/INode.h>

namespace ibis
{
    namespace ui
    {
        void INodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ibis::models::Document>& document,
            const std::shared_ptr<ibis::render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "ibis::INodeWidget", parent);
            _document = document;
            _node = node;
        }

        INodeWidget::~INodeWidget()
        {}

        const render::NodeInfo& INodeWidget::getNodeInfo() const
        {
            return _node->getNodeInfo();
        }

        const std::shared_ptr<ibis::render::INode>& INodeWidget::getNode() const
        {
            return _node;
        }

        struct IInteractionNodeWidget::Private
        {
            std::shared_ptr<render::NodeAttrCmd> cmd;
        };

        void IInteractionNodeWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ibis::models::Document>& document,
            const std::shared_ptr<ibis::render::INode>& node,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            INodeWidget::_init(context, document, node, parent);
        }

        IInteractionNodeWidget::IInteractionNodeWidget() :
            _p(new Private)
        {}

        IInteractionNodeWidget::~IInteractionNodeWidget()
        {}

        void IInteractionNodeWidget::_callback(
            const std::vector<std::pair<std::string, nlohmann::json> >&attr,
            bool pressed)
        {
            FTK_P();
            render::NodeAttr tmp = _node->getAttr();
            for (const auto& i : attr)
            {
                tmp[i.first] = i.second;
            }
            if (pressed)
            {
                if (!p.cmd)
                {
                    p.cmd = render::NodeAttrCmd::create(
                        _document->getGraph(),
                        _node,
                        tmp);
                }
                _document->getGraph()->setAttr(_node, tmp);
            }
            else if (p.cmd)
            {
                p.cmd->set(tmp);
                _document->command(p.cmd);
                p.cmd.reset();
            }
            else
            {
                _document->command(render::NodeAttrCmd::create(
                    _document->getGraph(),
                    _node,
                    tmp));
            }
        }
    }
}
