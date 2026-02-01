// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include "NodeMenu.h"

#include "App.h"
#include "MainWindow.h"
#include "DocumentWidget.h"

#include <ibis/Models/DocumentModel.h>

#include <ibis/Render/GraphCmd.h>
#include <ibis/Render/NodeFactory.h>

namespace ibis
{
    struct NodeMenu::Private
    {
    };

    void NodeMenu::_init(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<IWidget>& parent)
    {
        ftk::Menu::_init(context, parent);
        auto nodeFactory = app->getNodeFactory();
        auto appWeak = std::weak_ptr<App>(app);
        for (const auto& group : nodeFactory->getGroups())
        {
            auto groupMenu = addSubMenu(group);
            for (const auto& node : nodeFactory->getIDs(group))
            {
                const auto info = nodeFactory->getInfo(node);
                auto action = ftk::Action::create(
                    info.name,
                    [appWeak, info]
                    {
                        auto app = appWeak.lock();
                        auto document = app->getDocumentModel()->getCurrent();
                        auto documentWidget = app->getMainWindow()->getDocumentWidget();
                        if (document && documentWidget)
                        {
                            if (auto node = app->getNodeFactory()->createNode(info.id))
                            {
                                const ftk::Box2I& g = documentWidget->getCanvasViewRect();
                                document->command(
                                    render::AddNodesCmd::create(
                                        document->getGraph(),
                                        { node },
                                        { ftk::center(g) }));
                            }
                        }
                    });
                groupMenu->addAction(action);
            }
        }
    }

    NodeMenu::~NodeMenu()
    {}

    std::shared_ptr<NodeMenu> NodeMenu::create(
        const std::shared_ptr<ftk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<IWidget>& parent)
    {
        auto out = std::shared_ptr<NodeMenu>(new NodeMenu);
        out->_init(context, app, parent);
        return out;
    }
}