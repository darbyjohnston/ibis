// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <TestLib/ITest.h>

namespace ibis
{
    namespace render_test
    {
        class GraphCmdTest : public test::ITest
        {
        protected:
            GraphCmdTest(const std::shared_ptr<ftk::Context>&);

        public:
            virtual ~GraphCmdTest();

            static std::shared_ptr<GraphCmdTest> create(
                const std::shared_ptr<ftk::Context>&);

            void run() override;
        };
    }
}

