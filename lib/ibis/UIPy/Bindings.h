// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <pybind11/pybind11.h>

namespace ibis
{
    namespace python
    {
        void nodeBrowser(pybind11::module_&);
        void nodeEditor(pybind11::module_&);
        void nodeGraphCanvas(pybind11::module_&);
        void nodeGraphWidget(pybind11::module_&);
        void timelineWidget(pybind11::module_&);
        void viewport(pybind11::module_&);

        void ui(pybind11::module_&);
    }
}
