/*
    MIT License

    Copyright (c) 2022 Forschungszentrum Jülich / Jan André Reuter & Felix Matuschke.

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
#include "PLIHDF5/attributes.h"

template <typename T>
void PLI::HDF5::AttributeHandler::createAttribute(
    const std::string &attributeName, const T &content) {
  createAttribute(attributeName, &content, PLI::HDF5::Type::createType<T>());
}

template <typename T>
void PLI::HDF5::AttributeHandler::createAttribute(
    const std::string &attributeName, const std::vector<T> &content,
    const std::vector<hsize_t> &dimensions) {
  PLI::HDF5::Type attributeType = PLI::HDF5::Type::createType<T>();
  this->createAttribute(attributeName, content.data(), dimensions, attributeType);
}

template <typename T>
const std::vector<T> PLI::HDF5::AttributeHandler::getAttribute(
    const std::string &attributeName) const {
  hid_t attributeID = H5Aopen(this->m_id, attributeName.c_str(), H5P_DEFAULT);
  checkHDF5Ptr(attributeID, "H5Aopen");
  hid_t attributeType = PLI::HDF5::Type::createType<T>();
  checkHDF5Ptr(attributeType, "PLI::HDF5::Type");
  hid_t attributeSpace = H5Aget_space(attributeID);
  checkHDF5Ptr(attributeSpace, "H5Aget_space");
  int ndims = H5Sget_simple_extent_ndims(attributeSpace);
  std::vector<hsize_t> dims;
  dims.resize(ndims);
  H5Sget_simple_extent_dims(attributeSpace, dims.data(), nullptr);

  hsize_t numElements = 1;
  for (int i = 0; i < ndims; ++i) {
    numElements *= dims[i];
  }
  std::vector<T> returnContainer;
  returnContainer.resize(numElements);
  checkHDF5Call(H5Aread(attributeID, attributeType, returnContainer.data()), "H5Aread");

  checkHDF5Call(H5Sclose(attributeSpace), "H5Sclose");
  checkHDF5Call(H5Aclose(attributeID), "H5Aclose");

  return returnContainer;
}

template <typename T>
void PLI::HDF5::AttributeHandler::updateAttribute(
    const std::string &attributeName, const T &content) {
  updateAttribute(attributeName, content, PLI::HDF5::Type::createType<T>());
}

template <typename T>
void PLI::HDF5::AttributeHandler::updateAttribute(
    const std::string &attributeName, const std::vector<T> &content,
    const std::vector<hsize_t> &dimensions) {
  updateAttribute(attributeName, content.data(), dimensions,
                  Type::createType<T>());
}