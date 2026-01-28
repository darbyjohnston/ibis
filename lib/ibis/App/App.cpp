// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include "App.h"

#include "MainWindow.h"

#include <ibis/UI/NodeWidgetFactory.h>

#include <ibis/Models/DocumentModel.h>
#include <ibis/Models/TimeUnitsModel.h>

#include <ibis/Render/NodeFactory.h>

#include <ibis/UI/Init.h>
#include <ibis/UI/NodeWidgetFactory.h>

#include <ftk/UI/DialogSystem.h>
#include <ftk/UI/FileBrowser.h>
#include <ftk/Core/FileIO.h>
#include <ftk/Core/CmdLine.h>

namespace ibis
{
    struct App::Private
    {
        struct CmdLine
        {
            std::shared_ptr<ftk::CmdLineListArg<std::string> > inputs;
        };
        CmdLine cmdLine;

        std::shared_ptr<ftk::RecentFilesModel> recentFilesModel;
        std::shared_ptr<models::TimeUnitsModel> timeUnitsModel;
        std::shared_ptr<render::NodeFactory> nodeFactory;
        std::shared_ptr<ui::NodeWidgetFactory> nodeWidgetFactory;
        std::shared_ptr<models::DocumentModel> documentModel;

        std::shared_ptr<MainWindow> window;
    };

    void App::_init(
        const std::shared_ptr<ftk::Context>& context,
        std::vector<std::string>& argv)
    {
        FTK_P();
        
        p.cmdLine.inputs = ftk::CmdLineListArg<std::string>::create(
            "input",
            "One or more files to open.",
            true);

        ftk::App::_init(
            context,
            argv,
            "ibis",
            "ibis compositor",
            { p.cmdLine.inputs });
    }

    App::App() :
        _p(new Private)
    {}

    App::~App()
    {}

    std::shared_ptr<App> App::create(
        const std::shared_ptr<ftk::Context>& context,
        std::vector<std::string>& argv)
    {
        auto out = std::shared_ptr<App>(new App);
        out->_init(context, argv);
        return out;
    }

    const std::shared_ptr<ftk::RecentFilesModel>& App::getRecentFilesModel() const
    {
        return _p->recentFilesModel;
    }

    const std::shared_ptr<models::TimeUnitsModel>& App::getTimeUnitsModel() const
    {
        return _p->timeUnitsModel;
    }

    const std::shared_ptr<render::NodeFactory>& App::getNodeFactory() const
    {
        return _p->nodeFactory;
    }

    const std::shared_ptr<ui::NodeWidgetFactory>& App::getNodeWidgetFactory() const
    {
        return _p->nodeWidgetFactory;
    }

    const std::shared_ptr<models::DocumentModel>& App::getDocumentModel() const
    {
        return _p->documentModel;
    }

    void App::newDocument()
    {
        _p->documentModel->add(models::Document::create(_context));
    }

    void App::open(const ftk::Path& path)
    {
        FTK_P();
        try
        {
            const std::string fileName = path.get();
            auto fileIO = ftk::FileIO::create(fileName, ftk::FileMode::Read);
            const std::string s = ftk::read(fileIO);
            const nlohmann::json json = nlohmann::json::parse(s);
            auto document = models::Document::create(
                _context,
                std::filesystem::u8path(fileName),
                json,
                p.nodeFactory);
            p.documentModel->add(document);
            p.recentFilesModel->addRecent(fileName);
        }
        catch (const std::exception& e)
        {
            auto dialogSystem = _context->getSystem<ftk::DialogSystem>();
            dialogSystem->message("ERROR", e.what(), p.window);
        }
    }

    void App::open()
    {
        FTK_P();
        auto fileBrowserSystem = _context->getSystem<ftk::FileBrowserSystem>();
        fileBrowserSystem->open(
            p.window,
            [this](const ftk::Path& value)
            {
                open(value);
            });
    }

    void App::save()
    {
        FTK_P();
        if (auto document = p.documentModel->getCurrent())
        {
            try
            {
                auto fileIO = ftk::FileIO::create(document->getPath(), ftk::FileMode::Write);
                const nlohmann::json json = document->to_json();
                fileIO->write(json.dump(4));
            }
            catch (const std::exception& e)
            {
                auto dialogSystem = _context->getSystem<ftk::DialogSystem>();
                dialogSystem->message("ERROR", e.what(), p.window);
            }
        }
    }

    void App::run()
    {
        FTK_P();

        ui::init(_context);

        p.recentFilesModel = ftk::RecentFilesModel::create(_context);
        p.timeUnitsModel = models::TimeUnitsModel::create(_context);
        p.nodeFactory = render::NodeFactory::create(_context);
        p.nodeWidgetFactory = ui::NodeWidgetFactory::create(_context);
        p.documentModel = models::DocumentModel::create(_context);

        auto fileBrowserSystem = _context->getSystem<ftk::FileBrowserSystem>();
        fileBrowserSystem->setRecentFilesModel(p.recentFilesModel);

        p.window = MainWindow::create(
            _context,
            std::dynamic_pointer_cast<App>(shared_from_this()));
        
        for (const auto& input : p.cmdLine.inputs->getList())
        {
            open(ftk::Path(input));
        }

        ftk::App::run();
    }
}