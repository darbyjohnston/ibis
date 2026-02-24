// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include "App.h"

#include "MainWindow.h"

#include <ibis/UI/Init.h>
#include <ibis/UI/NodeWidgetFactory.h>

#include <ibis/Models/Document.h>
#include <ibis/Models/DocumentModel.h>
#include <ibis/Models/SettingsModel.h>
#include <ibis/Models/TimeModel.h>
#include <ibis/Models/TimeUnitsModel.h>

#include <ibis/Render/Graph.h>
#include <ibis/Render/NodeFactory.h>
#include <ibis/Render/OutputNode.h>

#include <ftk/UI/DiagModel.h>
#include <ftk/UI/DialogSystem.h>
#include <ftk/UI/FileBrowser.h>
#include <ftk/UI/ProgressDialog.h>
#include <ftk/UI/RecentFilesModel.h>
#include <ftk/UI/SysLogModel.h>
#include <ftk/GL/System.h>
#include <ftk/Core/CmdLine.h>
#include <ftk/Core/FileIO.h>
#include <ftk/Core/Format.h>
#include <ftk/Core/Timer.h>

namespace ibis
{
    struct App::Private
    {
        struct CmdLine
        {
            std::shared_ptr<ftk::CmdLineListArg<std::string> > inputs;
        };
        CmdLine cmdLine;

        std::shared_ptr<models::SettingsModel> settingsModel;
        std::shared_ptr<ftk::SysLogModel> sysLogModel;
        std::shared_ptr<ftk::DiagModel> diagModel;
        std::shared_ptr<ftk::RecentFilesModel> recentFilesModel;
        std::shared_ptr<models::TimeUnitsModel> timeUnitsModel;
        std::shared_ptr<render::NodeFactory> nodeFactory;
        std::shared_ptr<ui::NodeWidgetFactory> nodeWidgetFactory;
        std::shared_ptr<models::DocumentModel> documentModel;

        std::shared_ptr<MainWindow> window;

        struct Render
        {
            std::shared_ptr<render::Graph> graph;
            std::shared_ptr<ftk::IRender> render;
            OTIO_NS::TimeRange timeRange;
            double rate = 0.0;
            int64_t frame = 0;
            std::shared_ptr<ftk::ProgressDialog> dialog;
            std::shared_ptr<ftk::Timer> timer;
        };
        std::unique_ptr<Render> render;

        std::shared_ptr<ftk::Observer<models::StyleSettings> > styleSettingsObserver;
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
    {
        FTK_P();
        if (p.settingsModel)
        {
            p.settingsModel->setT("/MainWindow/Size", p.window->getSize());
            p.settingsModel->setT("/MainWindow/SidePanel", p.window->getSidePanel());
        }
    }

    std::shared_ptr<App> App::create(
        const std::shared_ptr<ftk::Context>& context,
        std::vector<std::string>& argv)
    {
        auto out = std::shared_ptr<App>(new App);
        out->_init(context, argv);
        return out;
    }

    const std::shared_ptr<models::SettingsModel>& App::getSettingsModel() const
    {
        return _p->settingsModel;
    }

    const std::shared_ptr<ftk::SysLogModel>& App::getSysLogModel() const
    {
        return _p->sysLogModel;
    }

    const std::shared_ptr<ftk::DiagModel>& App::getDiagModel() const
    {
        return _p->diagModel;
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

    const std::shared_ptr<MainWindow>& App::getMainWindow() const
    {
        return _p->window;
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

    void App::render()
    {
        FTK_P();
        if (auto document = p.documentModel->getCurrent())
        {
            try
            {
                _render(document);

                p.render->dialog = ftk::ProgressDialog::create(
                    _context,
                    "Render",
                    "Rendering:");
                p.render->dialog->setRange(0.0, p.render->timeRange.duration().value() - 1.0);
                p.render->dialog->setMessage(ftk::Format("Frame: {0} / {1}").
                    arg(p.render->timeRange.start_time().value()).
                    arg(p.render->timeRange.end_time_inclusive().value()));
                p.render->dialog->setCloseCallback([this]{ _p->render.reset(); });
                p.render->dialog->open(p.window);

                p.render->timer = ftk::Timer::create(_context);
                p.render->timer->setRepeating(true);
                p.render->timer->start(
                    std::chrono::microseconds(500),
                    [this]
                    {
                        FTK_P();
                        const int64_t start = p.render->timeRange.start_time().value();
                        p.render->dialog->setValue(p.render->frame - start);
                        p.render->dialog->setMessage(ftk::Format("Frame: {0} / {1}").
                            arg(p.render->frame - start).
                            arg(static_cast<int64_t>(p.render->timeRange.duration().value())));
                        bool error = false;
                        try
                        {
                            _renderFrame();
                        }
                        catch (const std::exception& e)
                        {
                            error = true;
                            _context->getSystem<ftk::DialogSystem>()->message(
                                "ERROR",
                                ftk::Format("Error: {0}").arg(e.what()),
                                p.window);
                        }
                        const int64_t end = p.render->timeRange.end_time_inclusive().value();
                        if (error || p.render->frame > end)
                        {
                            p.render->dialog->close();
                        }
                    });
            }
            catch (const std::exception& e)
            {
                _context->getSystem<ftk::DialogSystem>()->message(
                    "ERROR",
                    ftk::Format("Error: {0}").arg(e.what()),
                    p.window);
            }
        }
    }

    void App::run()
    {
        FTK_P();

        ui::init(_context);

        _createModels();
        _createWindow();
        _createObservers();

        for (const auto& input : p.cmdLine.inputs->getList())
        {
            open(ftk::Path(input));
        }

        if (!p.documentModel->getCurrent())
        {
            newDocument();
        }

        ftk::App::run();
    }

    void App::_createModels()
    {
        FTK_P();
        p.settingsModel = models::SettingsModel::create(
            _context,
            ftk::getSettingsPath("ibis", "ibis.json"),
            getDefaultDisplayScale());
        p.sysLogModel = ftk::SysLogModel::create(_context);
        p.diagModel = ftk::DiagModel::create(_context);
        p.recentFilesModel = ftk::RecentFilesModel::create(_context);
        auto fileBrowserSystem = _context->getSystem<ftk::FileBrowserSystem>();
        fileBrowserSystem->setRecentFilesModel(p.recentFilesModel);
        p.timeUnitsModel = models::TimeUnitsModel::create(_context, p.settingsModel);
        p.nodeFactory = render::NodeFactory::create(_context);
        p.nodeWidgetFactory = ui::NodeWidgetFactory::create(_context);
        p.documentModel = models::DocumentModel::create(_context);
    }

    void App::_createWindow()
    {
        FTK_P();
        ftk::Size2I windowSize(1700, 960);
        p.settingsModel->getT("/MainWindow/Size", windowSize);
        p.window = MainWindow::create(
            _context,
            std::dynamic_pointer_cast<App>(shared_from_this()),
            windowSize);
    }

    void App::_createObservers()
    {
        FTK_P();
        p.styleSettingsObserver = ftk::Observer<models::StyleSettings>::create(
            p.settingsModel->observeStyle(),
            [this](const models::StyleSettings& value)
            {
                getStyle()->setColorControls(value.colorControls);
                setColorStyle(value.colorStyle);
                setDisplayScale(value.displayScale);
            });
    }

    void App::_render(const std::shared_ptr<models::Document>& document)
    {
        FTK_P();
        p.render.reset(new Private::Render);
        p.render->graph = document->getGraph();
        p.render->render = _context->getSystem<ftk::gl::System>()->getRenderFactory()->createRender(
            _context->getLogSystem(),
            _context->getSystem<ftk::FontSystem>());
        const OTIO_NS::TimeRange& timeRange = document->getTimeModel()->getTimeRange();
        p.render->timeRange = timeRange;
        p.render->rate = timeRange.duration().rate();
        p.render->frame = timeRange.start_time().value();
    }

    void App::_renderFrame()
    {
        FTK_P();

        // Note that the size is arbitrary.
        p.render->render->begin(ftk::Size2I(512, 512));

        const OTIO_NS::RationalTime time(p.render->frame, p.render->rate);
        for (const auto& node : p.render->graph->getLeafNodes())
        {
            node->execInit(time);
        }
        for (const auto& node : p.render->graph->getLeafNodes())
        {
            node->exec(p.render->render, time);
        }

        for (const auto& node : p.render->graph->getNodes())
        {
            if (auto output = std::dynamic_pointer_cast<render::IOutputNode>(node))
            {
                output->write(time);
            }
        }

        p.render->render->end();
        ++(p.render->frame);
    }
}
