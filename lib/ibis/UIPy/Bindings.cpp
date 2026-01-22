// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "Bindings.h"

#include <ftk/Core/Context.h>

namespace py = pybind11;

namespace ibis
{
    namespace python
    {
        void ui(py::module_& m)
        {
            auto mUI = m.def_submodule("ui", "User interface");

            nodeBrowser(mUI);
            nodeEditor(mUI);
            nodeGraphCanvas(mUI);
            nodeGraphWidget(mUI);
            timelineWidget(mUI);
            viewport(mUI);
        }
    }
}

