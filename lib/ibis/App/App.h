// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#pragma once

#include <ftk/UI/App.h>
#include <ftk/UI/RecentFilesModel.h>
#include <ftk/Core/Path.h>

namespace ibis
{
    namespace render
    {
        class NodeFactory;
    }

    namespace models
    {
        class DocumentModel;
        class TimeUnitsModel;
    }

    namespace ui
    {
        class NodeWidgetFactory;
    }

    class MainWindow;

    //! Application.
    class App : public ftk::App
    {
        FTK_NON_COPYABLE(App);

    protected:
        void _init(
            const std::shared_ptr<ftk::Context>&,
            std::vector<std::string>&);

        App();

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

        const std::shared_ptr<MainWindow>& getMainWindow() const;

        void newDocument();
        void open(const ftk::Path&);
        void open();
        void save();

        void run() override;

    private:
        FTK_PRIVATE();
    };
}
