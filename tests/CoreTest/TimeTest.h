// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <TestLib/ITest.h>

namespace ibis
{
    namespace core_test
    {
        class TimeTest : public test::ITest
        {
        protected:
            TimeTest(const std::shared_ptr<ftk::Context>&);

        public:
            virtual ~TimeTest();

            static std::shared_ptr<TimeTest> create(
                const std::shared_ptr<ftk::Context>&);

            void run() override;
        };
    }
}

