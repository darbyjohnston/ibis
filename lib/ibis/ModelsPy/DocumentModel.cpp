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
                .def_property_readonly("documents", &DocumentModel::getDocuments)
                .def_property_readonly("observeDocuments", &DocumentModel::observeDocuments)
                .def("newDocument", &DocumentModel::newDocument)
                .def("openDocument", &DocumentModel::openDocument)
                .def_property("currentDocument", &DocumentModel::getCurrentDocument, &DocumentModel::setCurrentDocument)
                .def_property_readonly("observeCurrentDocument", &DocumentModel::observeCurrentDocument);
        }
    }
}
