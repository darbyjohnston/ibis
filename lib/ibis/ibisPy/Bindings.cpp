// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include <ibis/ibisModelsPy/Bindings.h>
#include <ibis/ibisRenderPy/Bindings.h>
#include <ibis/ibisUIPy/Bindings.h>

#include <opentimelineio/version.h>

#include <pybind11/pybind11.h>

#include <iostream>

namespace py = pybind11;

PYBIND11_MODULE(ibisPy, m)
{
    m.doc() = "ibis compositor";

    py::module_::import("opentimelineio");
    py::module_::import("ftkPy");

    ibis::python::modelsBind(m);
    ibis::python::renderBind(m);
    ibis::python::uiBind(m);
}

