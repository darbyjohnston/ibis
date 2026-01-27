// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#pragma once

#include <TestLib/ITest.h>

namespace ibis
{
    namespace render_test
    {
        class GraphTest : public test::ITest
        {
        protected:
            GraphTest(const std::shared_ptr<ftk::Context>&);

        public:
            virtual ~GraphTest();

            static std::shared_ptr<GraphTest> create(
                const std::shared_ptr<ftk::Context>&);

            void run() override;
        };
    }
}

