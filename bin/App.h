// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <ibis/UI/NodeWidgetFactory.h>

#include <ibis/Models/DocumentModel.h>
#include <ibis/Models/TimeUnitsModel.h>

#include <ibis/Render/NodeFactory.h>

#include <ftk/UI/App.h>
#include <ftk/UI/RecentFilesModel.h>
#include <ftk/Core/CmdLine.h>
#include <ftk/Core/Path.h>

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

        const std::shared_ptr<models::TimeUnitsModel>& getTimeUnitsModel() const;

        const std::shared_ptr<render::NodeFactory>& getNodeFactory() const;

        const std::shared_ptr<ui::NodeWidgetFactory>& getNodeWidgetFactory() const;

        const std::shared_ptr<models::DocumentModel>& getDocumentModel() const;

        void newDocument();
        void open(const ftk::Path&);
        void open();
        void save();

        void run() override;

    private:
        struct CmdLine
        {
            std::shared_ptr<ftk::CmdLineListArg<std::string> > inputs;
        };
        CmdLine _cmdLine;

        std::shared_ptr<ftk::RecentFilesModel> _recentFilesModel;
        std::shared_ptr<models::TimeUnitsModel> _timeUnitsModel;
        std::shared_ptr<render::NodeFactory> _nodeFactory;
        std::shared_ptr<ui::NodeWidgetFactory> _nodeWidgetFactory;
        std::shared_ptr<models::DocumentModel> _documentModel;

        std::shared_ptr<MainWindow> _window;
    };
}
