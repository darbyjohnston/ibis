// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "Bindings.h"

#include <ibis/UI/NodeBrowser.h>

#include <ftk/Core/Context.h>

namespace py = pybind11;

namespace ibis
{
    namespace python
    {
        void nodeBrowser(py::module_& m)
        {
            using namespace ui;

            py::class_<NodeBrowser, ftk::IWidget, std::shared_ptr<NodeBrowser> >(m, "NodeBrowser")
                .def(
                    py::init(py::overload_cast<
                        const std::shared_ptr<ftk::Context>&,
                        const std::shared_ptr<render::NodeFactory>&,
                        const std::shared_ptr<ftk::IWidget>&>(&NodeBrowser::create)),
                    py::arg("context"),
                    py::arg("factory"),
                    py::arg("parent") = nullptr);
        }
    }
}
