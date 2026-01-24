// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <TestLib/ITest.h>

namespace ibis
{
    namespace models_test
    {
        class DocumentTest : public test::ITest
        {
        protected:
            DocumentTest(const std::shared_ptr<ftk::Context>&);

        public:
            virtual ~DocumentTest();

            static std::shared_ptr<DocumentTest> create(
                const std::shared_ptr<ftk::Context>&);

            void run() override;
        };
    }
}

