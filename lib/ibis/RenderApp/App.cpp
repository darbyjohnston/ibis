// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include "App.h"

#include <ibis/Render/Graph.h>
#include <ibis/Render/NodeFactory.h>
#include <ibis/Render/OutputNode.h>
#include <ibis/Core/Time.h>

#include <ftk/GL/Init.h>
#include <ftk/GL/System.h>
#include <ftk/GL/Window.h>
#include <ftk/Core/CmdLine.h>
#include <ftk/Core/FileIO.h>
#include <ftk/Core/Format.h>
#include <ftk/Core/IRender.h>

namespace ibis
{
    struct App::Private
    {
        std::shared_ptr<ftk::CmdLineListArg<std::string> > inputs;
    };

    void App::_init(
        const std::shared_ptr<ftk::Context>& context,
        std::vector<std::string>& argv)
    {
        FTK_P();
        
        p.inputs = ftk::CmdLineListArg<std::string>::create(
            "input",
            "One or more files to open.",
            true);

        ftk::IApp::_init(
            context,
            argv,
            "ibis-render",
            "ibis render",
            { p.inputs });
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

    void App::run()
    {
        FTK_P();
        ftk::gl::init(_context);

        // Create a window for the OpenGL context.
        //
        // Note that the size is arbitrary.
        const ftk::Size2I size(512, 512);
        auto window = ftk::gl::Window::create(
            _context,
            "ibis-render",
            size,
            static_cast<int>(ftk::gl::WindowOptions::MakeCurrent));

        auto nodeFactory = render::NodeFactory::create(_context);
        auto render = _context->getSystem<ftk::gl::System>()->getRenderFactory()->createRender(
            _context->getLogSystem(),
            _context->getSystem<ftk::FontSystem>());

        // Loop over the inputs.
        for (const auto& input : p.inputs->getList())
        {
            _print(ftk::Format("Loading: {0}").arg(input));

            // Load the file.
            auto fileIO = ftk::FileIO::create(input, ftk::FileMode::Read);
            const std::string s = ftk::read(fileIO);
            nlohmann::json json = nlohmann::json::parse(s);
            OTIO_NS::TimeRange timeRange;
            if (json.contains("TimeRange"))
            {
                timeRange = json["TimeRange"];
            }
            auto graph = render::Graph::create(_context, json["Graph"], nodeFactory);

            // Loop over the frames.
            _print(ftk::Format("Frames: {0}-{1}").
                arg(int64_t(timeRange.start_time().value())).
                arg(int64_t(timeRange.end_time_inclusive().value())));
            const OTIO_NS::RationalTime timeInc(1.0, timeRange.duration().rate());
            for (OTIO_NS::RationalTime t = timeRange.start_time();
                t <= timeRange.end_time_inclusive();
                t += timeInc)
            {
                _print(ftk::Format("Rendering: {0}").arg(static_cast<int64_t>(t.value())));

                render->begin(size);

                // Execute the leaf nodes.
                for (const auto& node : graph->getLeafNodes())
                {
                    node->exec(render, t);
                }

                // Write the output nodes.
                for (const auto& node : graph->getNodes())
                {
                    if (auto output = std::dynamic_pointer_cast<render::IOutputNode>(node))
                    {
                        output->write(t);
                    }
                }

                render->end();
            }
        }
    }
}
