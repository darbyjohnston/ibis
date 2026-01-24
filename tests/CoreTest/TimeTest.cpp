// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include "TimeTest.h"

#include <ibis/Core/Time.h>

#include <ftk/Core/Assert.h>

namespace ibis
{
    namespace core_test
    {
        TimeTest::TimeTest(const std::shared_ptr<ftk::Context>& context) :
            ITest(context, "ibis::core_test::TimeTest")
        {}

        TimeTest::~TimeTest()
        {}

        std::shared_ptr<TimeTest> TimeTest::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            return std::shared_ptr<TimeTest>(new TimeTest(context));
        }

        void TimeTest::run()
        {
        }
    }
}
