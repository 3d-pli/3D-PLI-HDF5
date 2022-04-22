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

#include <string>

namespace PLI {
namespace HDF5 {
class Link {
  static bool createSoft(hid_t parentPtr, const std::string &path,
                         const std::string &target);
  static bool createHard(hid_t parentPtr, const std::string &path,
                         const std::string &target);
  static bool createExternal(hid_t parentPtr, const std::string &externalPath,
                             const std::string &path,
                             const std::string &target);
  static bool exists(hid_t parentPtr, const std::string &path);
  static bool isSoftLink(hid_t parentPtr, const std::string &path);
  static bool deleteLink(hid_t parentPtr, const std::string &path);
  static bool moveLink(hid_t parentPtr, const std::string &sourcePath,
                       const std::string &destinationPath);
  static bool copyLink(hid_t parentPtr, const std::string &sourcePath,
                       const std::string &destinationPath);
};
}  // namespace HDF5
}  // namespace PLI
