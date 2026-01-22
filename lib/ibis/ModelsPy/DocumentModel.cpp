// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "Bindings.h"

#include <ibis/Models/DocumentModel.h>

#include <ftk/Core/Context.h>

#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <pybind11/stl/filesystem.h>

namespace py = pybind11;

namespace ibis
{
    namespace python
    {
        void documentModel(py::module_& m)
        {
            using namespace models;

            py::class_<DocumentModel, std::shared_ptr<DocumentModel> >(m, "DocumentModel")
                .def(
                    py::init(py::overload_cast<const std::shared_ptr<ftk::Context>&>(&DocumentModel::create)),
                    py::arg("context"))
                .def_property_readonly("get", &DocumentModel::get)
                .def_property_readonly("observe", &DocumentModel::observe)
                .def("newDocument", &DocumentModel::newDocument)
                .def_property_readonly("observeNewDocument", &DocumentModel::observeNewDocument)
                .def("open", &DocumentModel::open)
                .def("close", &DocumentModel::close)
                .def("closeAll", &DocumentModel::closeAll);
        }
    }
}
