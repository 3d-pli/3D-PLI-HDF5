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

#pragma once

#include <hdf5.h>
#include <hdf5_hl.h>
#include <mpi.h>

#include <string>
#include <vector>

#include "PLIHDF5/type.h"

namespace PLI {
namespace HDF5 {
class Dataset {
 public:
  static PLI::HDF5::Dataset open(const hid_t parentPtr,
                                 const std::string& datasetName);
  template <typename T>
  static PLI::HDF5::Dataset create(const hid_t parentPtr,
                                   const std::string& datasetName,
                                   const int32_t ndims, const hsize_t* dims,
                                   const bool chunked = true);
  static bool exists(const hid_t parentPtr, const std::string& datasetName);

  void close();
  template <typename T>
  T* read() const;
  template <typename T>
  void write(const T* data, const int32_t ndims, const hsize_t* dims);

  const PLI::HDF5::Type type() const;
  int ndims() const;
  const std::vector<hsize_t> dims() const;
  hid_t id() const;

 private:
  explicit Dataset(hid_t datasetPtr);
  ~Dataset();
  hid_t m_id;
};
}  // namespace HDF5
}  // namespace PLI
