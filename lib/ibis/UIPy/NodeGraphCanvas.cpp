// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "Bindings.h"

#include <ibis/UI/NodeGraphCanvas.h>

#include <ftk/Core/Context.h>

namespace py = pybind11;

namespace ibis
{
    namespace python
    {
        void nodeGraphCanvas(py::module_& m)
        {
            using namespace ui;

            py::class_<NodeGraphCanvas, ftk::IWidget, std::shared_ptr<NodeGraphCanvas> >(m, "NodeGraphCanvas")
                .def(
                    py::init(py::overload_cast<
                        const std::shared_ptr<ftk::Context>&,
                        const std::shared_ptr<ftk::IWidget>&>(&NodeGraphCanvas::create)),
                    py::arg("context"),
                    py::arg("parent") = nullptr);
        }
    }
}
