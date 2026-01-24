// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include "App.h"

#include "MainWindow.h"

#include <ibis/UI/NodeWidgetFactory.h>

#include <ftk/UI/DialogSystem.h>
#include <ftk/UI/FileBrowser.h>
#include <ftk/Core/FileIO.h>

namespace ibis
{
    void App::_init(
        const std::shared_ptr<ftk::Context>& context,
        std::vector<std::string>& argv)
    {
        _cmdLine.inputs = ftk::CmdLineListArg<std::string>::create(
            "input",
            "One or more files to open.",
            true);

        ftk::App::_init(
            context,
            argv,
            "ibis",
            "ibis compositor",
            { _cmdLine.inputs });
    }

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
        return _recentFilesModel;
    }

    const std::shared_ptr<render::NodeFactory>& App::getNodeFactory() const
    {
        return _nodeFactory;
    }

    const std::shared_ptr<ui::NodeWidgetFactory>& App::getNodeWidgetFactory() const
    {
        return _nodeWidgetFactory;
    }

    const std::shared_ptr<models::DocumentModel>& App::getDocumentModel() const
    {
        return _documentModel;
    }

    void App::newDocument()
    {
        _documentModel->add(models::Document::create(_context));
    }

    void App::open(const ftk::Path& path)
    {
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
                _nodeFactory);
            _documentModel->add(document);
            _recentFilesModel->addRecent(fileName);
        }
        catch (const std::exception& e)
        {
            auto dialogSystem = _context->getSystem<ftk::DialogSystem>();
            dialogSystem->message("ERROR", e.what(), _window);
        }
    }

    void App::open()
    {
        auto fileBrowserSystem = _context->getSystem<ftk::FileBrowserSystem>();
        fileBrowserSystem->open(
            _window,
            [this](const ftk::Path& value)
            {
                open(value);
            });
    }

    void App::save()
    {
        if (auto document = _documentModel->getCurrent())
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
                dialogSystem->message("ERROR", e.what(), _window);
            }
        }
    }

    void App::run()
    {
        _recentFilesModel = ftk::RecentFilesModel::create(_context);

        _nodeFactory = render::NodeFactory::create(_context);

        _nodeWidgetFactory = ui::NodeWidgetFactory::create(_context);

        _documentModel = models::DocumentModel::create(_context);

        auto fileBrowserSystem = _context->getSystem<ftk::FileBrowserSystem>();
        fileBrowserSystem->setRecentFilesModel(_recentFilesModel);

        _window = MainWindow::create(
            _context,
            std::dynamic_pointer_cast<App>(shared_from_this()));
        
        for (const auto& input : _cmdLine.inputs->getList())
        {
            open(ftk::Path(input));
        }

        ftk::App::run();
    }
}