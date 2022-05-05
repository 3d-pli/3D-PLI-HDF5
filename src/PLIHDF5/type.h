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

/**
 * @brief The PLI namespace
 */
namespace PLI {
/**
 * @brief The HDF5 namespace
 */
namespace HDF5 {
class Type {
 public:
  template <typename T>
  static PLI::HDF5::Type createType();

  explicit Type(const std::string& typeName);
  explicit Type(const hid_t typeID);
  Type(const Type& type);

  operator hid_t() const;
  operator std::string() const;
  bool operator==(const Type& other) const;
  bool operator!=(const Type& other) const;

 private:
  static std::string convertIDToName(const hid_t ID);
  static hid_t convertNameToID(const std::string& name);

  hid_t m_typeID;
};
}  // namespace HDF5
}  // namespace PLI
