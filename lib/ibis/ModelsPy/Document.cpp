// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "Bindings.h"

#include <ibis/Models/Document.h>

#include <ftk/CorePy/Bindings.h>
#include <ftk/Core/Context.h>

#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <pybind11/stl/filesystem.h>

namespace py = pybind11;

namespace ibis
{
    namespace python
    {
        void document(py::module_& m)
        {
            using namespace models;

            ftk::python::observable<std::shared_ptr<Document> >(m, "Document");
            ftk::python::observableList<std::shared_ptr<Document> >(m, "Document");

            py::class_<Document, std::shared_ptr<Document> >(m, "Document")
                .def(
                    py::init(py::overload_cast<const std::shared_ptr<ftk::Context>&>(&Document::create)),
                    py::arg("context"))
                .def(
                    py::init(py::overload_cast<
                        const std::shared_ptr<ftk::Context>&,
                        const std::filesystem::path&>(&Document::create)),
                    py::arg("context"),
                    py::arg("path"))
                .def_property_readonly("path", &Document::getPath)
                .def_property_readonly("observePath", &Document::observePath)
                .def_property("timeRange", &Document::getTimeRange, &Document::setTimeRange)
                .def_property_readonly("observeTimeRange", &Document::observeTimeRange);
        }
    }
}
