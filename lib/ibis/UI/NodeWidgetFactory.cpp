// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "NodeWidgetFactory.h"

#include "CompNodeWidget.h"
#include "GenNodeWidget.h"
#include "InputNodeWidget.h"
#include "MathNodeWidget.h"

#include <ibis/Render/INode.h>

namespace ibis
{
    namespace ui
    {
        struct NodeWidgetFactory::Private
        {
            std::weak_ptr<ftk::Context> context;

            std::map<
                std::string,
                std::function<std::shared_ptr<INodeWidget>(
                    const std::shared_ptr<ftk::Context>&,
                    const std::shared_ptr<ibis::models::Document>&,
                    const std::shared_ptr<ibis::render::INode>&,
                    const std::shared_ptr<ftk::IWidget>&)> >
                nodes;
        };

        void NodeWidgetFactory::_init(const std::shared_ptr<ftk::Context>& context)
        {
            FTK_P();

            p.context = context;

            p.nodes[AddValueNodeWidget::getNodeID()] = &AddValueNodeWidget::create;
            p.nodes[InputNodeWidget::getNodeID()] = &InputNodeWidget::create;
            p.nodes[OverNodeWidget::getNodeID()] = &OverNodeWidget::create;
            p.nodes[SolidColorNodeWidget::getNodeID()] = &SolidColorNodeWidget::create;
        }

        NodeWidgetFactory::NodeWidgetFactory() :
            _p(new Private)
        {}

        NodeWidgetFactory::~NodeWidgetFactory()
        {}

        std::shared_ptr<NodeWidgetFactory> NodeWidgetFactory::create(const std::shared_ptr<ftk::Context>& context)
        {
            std::shared_ptr<NodeWidgetFactory> out(new NodeWidgetFactory);
            out->_init(context);
            return out;
        }

        std::vector<std::string> NodeWidgetFactory::getNodeIDs() const
        {
            FTK_P();
            std::vector<std::string> out;
            for (auto i = p.nodes.begin(); i != p.nodes.end(); ++i)
            {
                out.push_back(i->first);
            }
            return out;
        }

        std::shared_ptr<INodeWidget> NodeWidgetFactory::createWidget(
            const std::shared_ptr<models::Document>& document,
            const std::shared_ptr<render::INode>& node)
        {
            FTK_P();
            std::shared_ptr<INodeWidget> out;
            const auto i = p.nodes.find(node->getID());
            if (i != p.nodes.end())
            {
                out = i->second(p.context.lock(), document, node, nullptr);
            }
            return out;
        }
    }
}
