// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "Bindings.h"

#include <ibis/UI/Viewport.h>

#include <ftk/Core/Context.h>

namespace py = pybind11;

namespace ibis
{
    namespace python
    {
        void viewport(py::module_& m)
        {
            using namespace ui;

            py::class_<Viewport, ftk::IWidget, std::shared_ptr<Viewport> >(m, "Viewport")
                .def(
                    py::init(py::overload_cast<
                        const std::shared_ptr<ftk::Context>&,
                        const std::shared_ptr<ftk::IWidget>&>(&Viewport::create)),
                    py::arg("context"),
                    py::arg("parent") = nullptr);
        }
    }
}
