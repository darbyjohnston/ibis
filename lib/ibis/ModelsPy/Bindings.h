// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <pybind11/pybind11.h>

namespace ibis
{
    namespace python
    {
        void document(pybind11::module_&);
        void documentModel(pybind11::module_&);

        void models(pybind11::module_&);
    }
}
