// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include "DocumentTest.h"

#include <ibis/Models/Document.h>

#include <ftk/Core/Assert.h>

namespace ibis
{
    namespace models_test
    {
        DocumentTest::DocumentTest(const std::shared_ptr<ftk::Context>& context) :
            ITest(context, "ibis::models_test::DocumentTest")
        {}

        DocumentTest::~DocumentTest()
        {}

        std::shared_ptr<DocumentTest> DocumentTest::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            return std::shared_ptr<DocumentTest>(new DocumentTest(context));
        }

        void DocumentTest::run()
        {
            auto document = models::Document::create(_context);
        }
    }
}
