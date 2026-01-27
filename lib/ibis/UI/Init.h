// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <memory>

namespace ftk
{
    class Context;
}

namespace ibis
{
    namespace ui
    {
        void init(const std::shared_ptr<ftk::Context>&);
    }
}
