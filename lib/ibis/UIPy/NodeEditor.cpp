// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "Bindings.h"

#include <ibis/UI/NodeEditor.h>

#include <ftk/Core/Context.h>

namespace py = pybind11;

namespace ibis
{
    namespace python
    {
        void nodeEditor(py::module_& m)
        {
            using namespace ui;

            py::class_<NodeEditor, ftk::IWidget, std::shared_ptr<NodeEditor> >(m, "NodeEditor")
                .def(
                    py::init(py::overload_cast<
                        const std::shared_ptr<ftk::Context>&,
                        const std::shared_ptr<ftk::IWidget>&>(&NodeEditor::create)),
                    py::arg("context"),
                    py::arg("parent") = nullptr);
        }
    }
}
