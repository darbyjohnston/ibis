// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include <ibis/ModelsPy/Bindings.h>

#include <ftk/Core/Context.h>

namespace py = pybind11;

namespace ibis
{
    namespace python
    {
        void models(py::module_& m)
        {
            auto mModels = m.def_submodule("models", "Models");

            document(mModels);
            documentModel(mModels);
        }
    }
}
