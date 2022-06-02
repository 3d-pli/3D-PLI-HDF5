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

#include "PLIHDF5/type.h"

PLI::HDF5::Type::Type(const std::string &typeName) {
  this->m_typeID = PLI::HDF5::Type::convertNameToID(typeName);
}

PLI::HDF5::Type::Type(const hid_t typeID) { this->m_typeID = typeID; }

PLI::HDF5::Type::Type(const Type &type) { this->m_typeID = hid_t(type); }

PLI::HDF5::Type::operator hid_t() const { return this->m_typeID; }

PLI::HDF5::Type::operator std::string() const {
  return PLI::HDF5::Type::convertIDToName(this->m_typeID);
}

bool PLI::HDF5::Type::operator==(const Type &other) const {
  return H5Tequal(this->m_typeID, other) > 0;
}

bool PLI::HDF5::Type::operator!=(const Type &other) const {
  return !(*this == other);
}

PLI::HDF5::Type &PLI::HDF5::Type::operator=(const Type &other) {
  this->m_typeID = other.m_typeID;
  return *this;
}

std::string PLI::HDF5::Type::convertIDToName(const hid_t ID) {
  size_t typeNameLength = 0;
  // Make a first call to get the length of the typename
  checkHDF5Call(H5LTdtype_to_text(ID, nullptr, H5LT_DDL, &typeNameLength),
                "H5LTdtype_to_text");

  // Create a container to save the second call.
  std::string returnString;
  returnString.resize(typeNameLength);
  checkHDF5Call(
      H5LTdtype_to_text(ID, returnString.data(), H5LT_DDL, &typeNameLength),
      "H5LTdtype_to_text");

  return returnString;
}

hid_t PLI::HDF5::Type::convertNameToID(const std::string &name) {
  return H5LTtext_to_dtype(name.c_str(), H5LT_DDL);
}

// Unsigned Int
template <>
PLI::HDF5::Type PLI::HDF5::Type::createType<uint8_t>() {
  return PLI::HDF5::Type("H5T_NATIVE_UCHAR");
}

template <>
PLI::HDF5::Type PLI::HDF5::Type::createType<uint16_t>() {
  return PLI::HDF5::Type("H5T_NATIVE_USHORT");
}

template <>
PLI::HDF5::Type PLI::HDF5::Type::createType<uint32_t>() {
  return PLI::HDF5::Type("H5T_NATIVE_UINT");
}

template <>
PLI::HDF5::Type PLI::HDF5::Type::createType<uint64_t>() {
  return PLI::HDF5::Type("H5T_NATIVE_ULONG");
}

// Int
template <>
PLI::HDF5::Type PLI::HDF5::Type::createType<int8_t>() {
  return PLI::HDF5::Type("H5T_NATIVE_SCHAR");
}

template <>
PLI::HDF5::Type PLI::HDF5::Type::createType<int16_t>() {
  return PLI::HDF5::Type("H5T_NATIVE_SHORT");
}

template <>
PLI::HDF5::Type PLI::HDF5::Type::createType<int32_t>() {
  return PLI::HDF5::Type("H5T_NATIVE_INT");
}

template <>
PLI::HDF5::Type PLI::HDF5::Type::createType<int64_t>() {
  return PLI::HDF5::Type("H5T_NATIVE_LONG");
}

// Float
template <>
PLI::HDF5::Type PLI::HDF5::Type::createType<float>() {
  return PLI::HDF5::Type("H5T_NATIVE_FLOAT");
}

template <>
PLI::HDF5::Type PLI::HDF5::Type::createType<double>() {
  return PLI::HDF5::Type("H5T_NATIVE_DOUBLE");
}
