// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#pragma once

#include <ftk/UI/Menu.h>

namespace ibis
{
    class App;

    //! Node menu.
    class NodeMenu : public ftk::Menu
    {
        FTK_NON_COPYABLE(NodeMenu);

    protected:
        void _init(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent);

        NodeMenu() = default;

    public:
        ~NodeMenu();

        static std::shared_ptr<NodeMenu> create(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

    private:
        FTK_PRIVATE();
    };
}
