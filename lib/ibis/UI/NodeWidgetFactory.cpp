// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "NodeWidgetFactory.h"

#include "ColorNodeWidget.h"
#include "CompNodeWidget.h"
#include "GenNodeWidget.h"
#include "InputNodeWidget.h"
#include "MathNodeWidget.h"
#include "OutputNodeWidget.h"
#include "TransformNodeWidget.h"

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

#if defined(IBIS_OIIO)
            p.nodes[ImageInputNodeWidget::getClassNodeInfo().id] = &ImageInputNodeWidget::create;
            p.nodes[ImageOutputNodeWidget::getClassNodeInfo().id] = &ImageOutputNodeWidget::create;
            p.nodes[SequenceInputNodeWidget::getClassNodeInfo().id] = &SequenceInputNodeWidget::create;
            p.nodes[SequenceOutputNodeWidget::getClassNodeInfo().id] = &SequenceOutputNodeWidget::create;
#endif // IBIS_OIIO
#if defined(IBIS_USD)
            p.nodes[USDInputNodeWidget::getClassNodeInfo().id] = &USDInputNodeWidget::create;
#endif // IBIS_USD
            p.nodes[SVGInputNodeWidget::getClassNodeInfo().id] = &SVGInputNodeWidget::create;

            p.nodes[GradientNodeWidget::getClassNodeInfo().id] = &GradientNodeWidget::create;
            p.nodes[NoiseNodeWidget::getClassNodeInfo().id] = &NoiseNodeWidget::create;
            p.nodes[SolidColorNodeWidget::getClassNodeInfo().id] = &SolidColorNodeWidget::create;
            p.nodes[TextNodeWidget::getClassNodeInfo().id] = &TextNodeWidget::create;

            p.nodes[ArithmeticNodeWidget::getClassNodeInfo().id] = &ArithmeticNodeWidget::create;

            p.nodes[BrightnessNodeWidget::getClassNodeInfo().id] = &BrightnessNodeWidget::create;
            p.nodes[ContrastNodeWidget::getClassNodeInfo().id] = &ContrastNodeWidget::create;
            p.nodes[HueNodeWidget::getClassNodeInfo().id] = &HueNodeWidget::create;
            p.nodes[LevelsNodeWidget::getClassNodeInfo().id] = &LevelsNodeWidget::create;
            p.nodes[SaturationNodeWidget::getClassNodeInfo().id] = &SaturationNodeWidget::create;
            p.nodes[SoftClipNodeWidget::getClassNodeInfo().id] = &SoftClipNodeWidget::create;

            p.nodes[CropNodeWidget::getClassNodeInfo().id] = &CropNodeWidget::create;
            p.nodes[MirrorNodeWidget::getClassNodeInfo().id] = &MirrorNodeWidget::create;
            p.nodes[ResizeNodeWidget::getClassNodeInfo().id] = &ResizeNodeWidget::create;
            p.nodes[RotateNodeWidget::getClassNodeInfo().id] = &RotateNodeWidget::create;

            p.nodes[OverNodeWidget::getClassNodeInfo().id] = &OverNodeWidget::create;
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
            const auto i = p.nodes.find(node->getNodeInfo().id);
            if (i != p.nodes.end())
            {
                out = i->second(p.context.lock(), document, node, nullptr);
            }
            return out;
        }
    }
}
