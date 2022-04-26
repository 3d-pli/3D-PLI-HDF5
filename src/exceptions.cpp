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

#include "PLIHDF5/exceptions.h"

void PLI::HDF5::checkHDF5Call(const herr_t hdf5ReturnValue,
                              const std::string& message) {
  if (hdf5ReturnValue <= 0) {
    throw PLI::HDF5::HDF5RuntimeException(
        "[" + message + "]: Call of function returned value " +
        std::to_string(hdf5ReturnValue) + ", which is not successful.");
  }
}

void PLI::HDF5::checkHDF5Ptr(const hid_t hdf5Ptr, const std::string& message) {
  if (hdf5Ptr <= 0) {
    throw PLI::HDF5::IdentifierNotValidException(
        "[" + message + "]: Pointer to HDF5 object is invalid.");
  }
}
