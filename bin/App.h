// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <ibis/Render/NodeFactory.h>

#include <ibis/Models/DocumentModel.h>

#include <ftk/UI/App.h>
#include <ftk/UI/RecentFilesModel.h>
#include <ftk/Core/CmdLine.h>

namespace ibis
{
    class MainWindow;

    //! Application.
    class App : public ftk::App
    {
        FTK_NON_COPYABLE(App);

    protected:
        void _init(
            const std::shared_ptr<ftk::Context>&,
            std::vector<std::string>&);

        App() = default;

    public:
        ~App();

        static std::shared_ptr<App> create(
            const std::shared_ptr<ftk::Context>&,
            std::vector<std::string>&);

        const std::shared_ptr<ftk::RecentFilesModel>& getRecentFilesModel() const;

        const std::shared_ptr<render::NodeFactory>& getNodeFactory() const;

        const std::shared_ptr<models::DocumentModel>& getDocumentModel() const;

        void run() override;

    private:
        struct CmdLine
        {
        };
        CmdLine _cmdLine;

        std::shared_ptr<ftk::RecentFilesModel> _recentFilesModel;
        std::shared_ptr<render::NodeFactory> _nodeFactory;
        std::shared_ptr<models::DocumentModel> _documentModel;

        std::shared_ptr<MainWindow> _window;
    };
}
