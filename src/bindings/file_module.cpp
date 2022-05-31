/*
    MIT License

    Copyright (c) 2022 Forschungszentrum Jülich / Jan André Reuter & Felix
   Matuschke.

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
   deal in the Software without restriction, including without limitation the
   rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
   sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
   IN THE SOFTWARE.
 */

#include <pybind11/pybind11.h>

#include "../PLIHDF5/dataset.h"
#include "../PLIHDF5/file.h"

namespace py = pybind11;

PYBIND11_MODULE(pli_hdf5, m) {
  m.doc() = "PLI HDF5";

  // TODO(Felix): mpi_init
  py::class_<PLI::HDF5::File>(m, "File")
      .def(py::init())
      .def("create", &PLI::HDF5::File::create, py::arg("file_name"))
      .def(
          "open",
          [](PLI::HDF5::File &self, const std::string &file_name,
             const std::string &open_state) {
            PLI::HDF5::File::OpenState open_state_enum;
            if (open_state == "r")
              open_state_enum = PLI::HDF5::File::OpenState::ReadOnly;
            else if (open_state == "r+")
              open_state_enum = PLI::HDF5::File::OpenState::ReadWrite;
            else
              throw std::invalid_argument("Only 'r' and 'r+' are supported");
            return self.open(file_name, open_state_enum);
          },
          py::arg("file_name"), py::arg("open_state"))
      .def("isHDF5",
           &PLI::HDF5::File::isHDF5)  // FIXME(felix): py::arg("file_name")
      .def("file_exists",
           &PLI::HDF5::File::fileExists)  // FIXME(felix): py::arg("file_name")
      .def("close", &PLI::HDF5::File::close)
      .def("reopen", &PLI::HDF5::File::reopen)
      .def("flush", &PLI::HDF5::File::flush)
      .def_property_readonly("id", &PLI::HDF5::File::id)
      .def_property_readonly("faplID", &PLI::HDF5::File::faplID);

  py::class_<PLI::HDF5::Dataset>(m, "Dataset")
      .def(py::init())
      .def_property_readonly("id", &PLI::HDF5::Dataset::id);

  // # TODO(Felix): add version from cmake
  // m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
}
