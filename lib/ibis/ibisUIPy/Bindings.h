// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <pybind11/pybind11.h>

namespace ibis
{
    namespace python
    {
        void timelineWidget(pybind11::module_&);

        void ui(pybind11::module_&);
    }
}
