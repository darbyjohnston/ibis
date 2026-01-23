// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include "App.h"

#include "MainWindow.h"

#include <ftk/UI/DialogSystem.h>
#include <ftk/UI/FileBrowser.h>
#include <ftk/Core/Path.h>

namespace ibis
{
    void App::_init(
        const std::shared_ptr<ftk::Context>& context,
        std::vector<std::string>& argv)
    {
        ftk::App::_init(
            context,
            argv,
            "ibis",
            "ibis compositor");
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

    const std::shared_ptr<models::DocumentModel>& App::getDocumentModel() const
    {
        return _documentModel;
    }

    void App::run()
    {
        _recentFilesModel = ftk::RecentFilesModel::create(_context);

        _nodeFactory = render::NodeFactory::create(_context);

        _documentModel = models::DocumentModel::create(_context);
        _documentModel->newDocument();
        _documentModel->newDocument();

        auto fileBrowserSystem = _context->getSystem<ftk::FileBrowserSystem>();
        fileBrowserSystem->setRecentFilesModel(_recentFilesModel);

        _window = MainWindow::create(
            _context,
            std::dynamic_pointer_cast<App>(shared_from_this()));

        ftk::App::run();
    }
}