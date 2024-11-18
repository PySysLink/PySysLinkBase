#include <pybind11/pybind11.h>
#include "../PySysLinkBase/SystemBlock.h"

namespace py = pybind11;

// pybind11 module definition
PYBIND11_MODULE(pysyslink, m) {
    py::class_<SystemBlock>(m, "SystemBlock")
        .def(py::init<>())   // Constructor binding
        .def("simulate", &SystemBlock::simulate);  // Method binding
}