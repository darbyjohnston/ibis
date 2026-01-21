// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <ftk/Core/Context.h>

#include <iostream>
#include <vector>

int main(int argc, char* argv[])
{
    auto context = ftk::Context::create();

    auto logObserver = ftk::ListObserver<ftk::LogItem>::create(
        context->getSystem<ftk::LogSystem>()->observeLogItems(),
        [](const std::vector<ftk::LogItem>& value)
        {
            for (const auto& i : value)
            {
                std::cout << "[LOG] " << ftk::getLabel(i) << std::endl;
            }
        },
        ftk::ObserverAction::Suppress);

    context->tick();

    std::cout << "Finished tests" << std::endl;
    return 0;
}
