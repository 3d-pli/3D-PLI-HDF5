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
  updateAttribute(attributeName, content, PLI::HDF5::Type::createType<T>());
}

template <typename T>
void PLI::HDF5::AttributeHandler::updateAttribute(
    const std::string &attributeName, const std::vector<T> &content,
    const std::vector<hsize_t> &dimensions) {
  updateAttribute(attributeName, content.data(), dimensions,
                  Type::createType<T>());
}