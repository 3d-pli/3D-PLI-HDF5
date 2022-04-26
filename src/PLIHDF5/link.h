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

#include "PLIHDF5/exceptions.h"

namespace PLI {
namespace HDF5 {
class Link {
 public:
  static void createSoft(const hid_t parentPtr, const std::string &srcPath,
                         const std::string &dstPath);
  static void createHard(const hid_t parentPtr, const std::string &srcPath,
                         const std::string &dstPath);
  static void createExternal(const hid_t parentPtr,
                             const std::string &externalPath,
                             const std::string &srcPath,
                             const std::string &dstPath);
  static H5L_info_t getLinkInfo(const hid_t parentPtr, const std::string &path);
  static bool exists(const hid_t parentPtr, const std::string &path);
  static bool isSoftLink(const hid_t parentPtr, const std::string &path);
  static bool isHardLink(const hid_t parentPtr, const std::string &path);
  static bool isExternalLink(const hid_t parentPtr, const std::string &path);
  static void deleteLink(const hid_t parentPtr, const std::string &path);
  static void moveLink(const hid_t parentPtr, const std::string &srcPath,
                       const std::string &dstPath);
  static void copyLink(const hid_t parentPtr, const std::string &srcPath,
                       const std::string &dstPath);

 private:
  static void checkHDF5Call(herr_t returnValue);
};
}  // namespace HDF5
}  // namespace PLI
