// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include <ModelsTest/DocumentTest.h>

#include <RenderTest/GraphCmdTest.h>
#include <RenderTest/GraphTest.h>

#include <CoreTest/TimeTest.h>

#include <ftk/UI/Init.h>

#include <ftk/Core/CmdLine.h>
#include <ftk/Core/Context.h>
#include <ftk/Core/Format.h>
#include <ftk/Core/IApp.h>
#include <ftk/Core/Time.h>

#include <iostream>

namespace ibis
{
    namespace tests
    {
        class App : public ftk::IApp
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                std::vector<std::string>& argv);

            App() = default;

        public:
            virtual ~App() = default;

            static std::shared_ptr<App> create(
                const std::shared_ptr<ftk::Context>&,
                std::vector<std::string>&);

            void run() override;

        private:
            std::shared_ptr<ftk::CmdLineArg<std::string> > _testName;
            std::vector<std::shared_ptr<test::ITest> > _tests;
            std::chrono::steady_clock::time_point _startTime;
        };

        void App::_init(
            const std::shared_ptr<ftk::Context>& context,
            std::vector<std::string>& argv)
        {
            _testName = ftk::CmdLineArg<std::string>::create(
                "Test",
                "Name of the test to run.",
                true);
            IApp::_init(
                context,
                argv,
                "ibistest",
                "Test application",
                { _testName });
            _startTime = std::chrono::steady_clock::now();
            ftk::uiInit(context);

            _tests.push_back(core_test::TimeTest::create(context));

            _tests.push_back(render_test::GraphCmdTest::create(context));
            _tests.push_back(render_test::GraphTest::create(context));

            _tests.push_back(models_test::DocumentTest::create(context));
        }

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
            // Get the tests to run.
            std::vector<std::shared_ptr<test::ITest> > runTests;
            for (const auto& test : _tests)
            {
                if (!_testName->hasValue() ||
                    (_testName->hasValue() &&
                        ftk::contains(test->getName(), _testName->getValue())))
                {
                    runTests.push_back(test);
                }
            }

            // Run the tests.
            for (const auto& test : runTests)
            {
                _context->tick();
                _print(ftk::Format("Running test: {0}").arg(test->getName()));
                test->run();
            }

            const auto now = std::chrono::steady_clock::now();
            const std::chrono::duration<float> diff = now - _startTime;
            _print(ftk::Format("Seconds elapsed: {0}").arg(diff.count(), 2));
        }
    }
}

FTK_MAIN()
{
    int r = 0;
    try
    {
        auto context = ftk::Context::create();
        auto args = ftk::convert(argc, argv);
        auto app = ibis::tests::App::create(context, args);
        r = app->getExit();
        if (0 == r)
        {
            app->run();
        }
    }
    catch (const std::exception& e)
    {
        std::cout << "ERROR: " << e.what() << std::endl;
    }
    return r;
}

