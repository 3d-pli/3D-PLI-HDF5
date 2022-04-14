/*
    MIT License

    Copyright (c) 2022 Forschungszentrum Jülich / Jan André Reuter.

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
#include <mpi.h>

#ifdef __GNUC__
#include <pwd.h>
#include <unistd.h>
#else
#define NOMINMAX
#include <Windows.h>
#include <lmcons.h>
#endif

#include <memory>
#include <string>
#include <vector>

#include "PLIHDF5/type.h"

namespace PLI {
namespace HDF5 {
class AttributeHandler {
 public:
  explicit AttributeHandler(const hid_t parentPtr);

  template <typename T>
  void createAttribute(const std::string& attributeName,
                       const std::vector<T>& content,
                       const std::vector<hsize_t>& dimensions);
  template <typename T>
  void createAttribute(const std::string& attributeName, const T& content);
  template <typename T>
  const std::vector<T> getAttribute(const std::string& attributeName) const;
  const std::vector<hsize_t> getAttributeDimensions(
      const std::string& attributeName) const;
  template <typename T>
  void updateAttribute(const std::string& attributeName, const T& content);
  template <typename T>
  void updateAttribute(const std::string& attributeName,
                       const std::vector<T>& content,
                       const std::vector<hsize_t>& dimensions);
  void deleteAttribute(const std::string& attributeName);
  bool attributeExists(const std::string& attributeName) const;
  PLI::HDF5::Type attributeType(const std::string& attributeName) const;

  void copyTo(PLI::HDF5::AttributeHandler dstHandler,
              const std::string& srcName, const std::string& dstName);
  void copyFrom(const PLI::HDF5::AttributeHandler& srcHandler,
                const std::string& srcName, const std::string& dstName);
  void copyAllFrom(
      const PLI::HDF5::AttributeHandler& srcHandler,
      const std::vector<std::string>& exceptions = std::vector<std::string>());
  void copyAllTo(
      PLI::HDF5::AttributeHandler dstHandler,
      const std::vector<std::string>& exceptions = std::vector<std::string>());

  std::vector<std::string> attributeNames();
  hid_t id();

 private:
  hid_t m_id;
};
}  // namespace HDF5
}  // namespace PLI
