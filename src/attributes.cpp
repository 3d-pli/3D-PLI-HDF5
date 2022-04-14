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

#include "PLIHDF5/attributes.h"

void PLI::HDF5::AttributeHandler::deleteAttribute(
    const std::string &attributeName) {
  herr_t returnValue = H5Adelete(this->m_id, attributeName.c_str());
  if (returnValue < 0) {
    throw 0;
  }
}

bool PLI::HDF5::AttributeHandler::attributeExists(
    const std::string &attributeName) const {
  return H5Aexists(this->m_id, attributeName.c_str()) > 0;
}

PLI::HDF5::Type PLI::HDF5::AttributeHandler::attributeType(
    const std::string &attributeName) const {
  hid_t attribute = H5Aopen(m_id, attributeName.c_str(), H5P_DEFAULT);
  hid_t attributeType = H5Aget_type(attribute);
  PLI::HDF5::Type returnValue(attributeType);
  H5Aclose(attribute);
  return returnValue;
}

void PLI::HDF5::AttributeHandler::copyTo(AttributeHandler dstHandler,
                                         const std::string &srcName,
                                         const std::string &dstName) {}

void PLI::HDF5::AttributeHandler::copyFrom(const AttributeHandler &srcHandler,
                                           const std::string &srcName,
                                           const std::string &dstName) {}

void PLI::HDF5::AttributeHandler::copyAllFrom(
    const AttributeHandler &srcHandler,
    const std::vector<std::string> &exceptions) {}

void PLI::HDF5::AttributeHandler::copyAllTo(
    AttributeHandler dstHandler, const std::vector<std::string> &exceptions) {}

std::vector<std::string> PLI::HDF5::AttributeHandler::attributeNames() {
  int numAttrs = H5Aget_num_attrs(this->m_id);
  std::vector<std::string> attributes;
  attributes.resize(numAttrs);

  std::string attrName;
  for (int i = 0; i < numAttrs; ++i) {
    // Get length of the attribute name
    ssize_t nameLength =
        H5Aget_name_by_idx(this->m_id, ".", H5_INDEX_NAME, H5_ITER_INC,
                           hsize_t(i), nullptr, 0, H5P_DEFAULT);
    // Create attribute with the length
    attrName = std::string();
    attrName.resize(nameLength);
    // Get the actual attribute name
    H5Aget_name_by_idx(this->m_id, ".", H5_INDEX_NAME, H5_ITER_INC, hsize_t(i),
                       attrName.data(), nameLength, H5P_DEFAULT);
    attributes.at(i) = std::string(attrName);
  }

  return attributes;
}

hid_t PLI::HDF5::AttributeHandler::id() { return this->m_id; }

template <typename T>
void PLI::HDF5::AttributeHandler::createAttribute(
    const std::string &attributeName, const T &content) {
  hid_t attributeType = PLI::HDF5::Type::createType<T>();
  hid_t attrPtr = H5Acreate(m_id, attributeName.c_str(), attributeType,
                            H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  H5Awrite(attrPtr, attributeType, &content);
}

template <typename T>
const std::vector<T> PLI::HDF5::AttributeHandler::getAttribute(
    const std::string &attributeName) const {
  hid_t attributeID = H5Aopen(this->m_id, attributeName.c_str(), H5P_DEFAULT);
  hid_t attributeType = PLI::HDF5::Type::createType<T>();
  hid_t attributeSpace = H5Aget_space(attributeID);
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
  H5Aread(attributeID, attributeType, returnContainer.data());

  H5Sclose(attributeSpace);
  H5Aclose(attributeID);

  return returnContainer;
}

template <typename T>
void PLI::HDF5::AttributeHandler::updateAttribute(
    const std::string &attributeName, const T &content) {
  if (!this->attributeExists(attributeName)) {
    throw 0;
  }

  hid_t attributeType = PLI::HDF5::Type::createType<T>();
  hid_t attributeID = H5Aopen(this->m_id, attributeName.c_str(), H5P_DEFAULT);
  H5Awrite(attributeID, attributeType, &content);
  H5Aclose(attributeID);
}

template <typename T>
void PLI::HDF5::AttributeHandler::createAttribute(
    const std::string &attributeName, const std::vector<T> &content,
    const std::vector<hsize_t> &dimensions) {
  hid_t attributeType = PLI::HDF5::Type::createType<T>();
  hid_t spacePtr =
      H5Screate_simple(dimensions.size(), dimensions.data(), nullptr);
  hid_t attrPtr = H5Acreate(m_id, attributeName.c_str(), attributeType,
                            spacePtr, H5P_DEFAULT, H5P_DEFAULT);
  H5Awrite(attrPtr, attributeType, content.data());
}
