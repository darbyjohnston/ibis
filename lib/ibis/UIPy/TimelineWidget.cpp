// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "Bindings.h"

#include <ibis/UI/TimelineWidget.h>

#include <ftk/Core/Context.h>

namespace py = pybind11;

namespace ibis
{
    namespace python
    {
        void timelineWidget(py::module_& m)
        {
            using namespace ui;

            py::class_<TimelineWidget, ftk::IWidget, std::shared_ptr<TimelineWidget> >(m, "TimelineWidget")
                .def(
                    py::init(py::overload_cast<
                        const std::shared_ptr<ftk::Context>&,
                        const std::shared_ptr<ftk::IWidget>&>(&TimelineWidget::create)),
                    py::arg("context"),
                    py::arg("parent") = nullptr);
        }
    }
}
