// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include "App.h"

int main(int argc, char* argv[])
{
    int r = 1;
    try
    {
        auto context = ftk::Context::create();
        auto args = ftk::convert(argc, argv);
        auto app = ibis::App::create(context, args);
        r = app->getExit();
        if (0 == r)
        {
            app->run();
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }
    return r;
}
