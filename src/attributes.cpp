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

#include "PLIHDF5/attributes.h"

PLI::HDF5::AttributeHandler::AttributeHandler() noexcept : m_id(-1) {}

PLI::HDF5::AttributeHandler::AttributeHandler(const hid_t parentPtr) noexcept
    : m_id(parentPtr) {}

void PLI::HDF5::AttributeHandler::setPtr(const hid_t parentPtr) noexcept {
  m_id = parentPtr;
}

bool PLI::HDF5::AttributeHandler::attributeExists(
    const std::string &attributeName) const {
  checkHDF5Ptr(this->m_id, "AttributeHandler");
  return H5Aexists(this->m_id, attributeName.c_str()) > 0;
}

const std::vector<std::string> PLI::HDF5::AttributeHandler::attributeNames()
    const {
  checkHDF5Ptr(this->m_id, "AttributeHandler");
  int numAttrs = H5Aget_num_attrs(this->m_id);
  if (numAttrs < 0) {
    throw HDF5RuntimeException("Could not get the number of attributes.");
  }
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

PLI::HDF5::Type PLI::HDF5::AttributeHandler::attributeType(
    const std::string &attributeName) const {
  if (!this->attributeExists(attributeName)) {
    throw AttributeNotFoundException(
        "Could not update attribute because it "
        "does not exist.");
  }
  hid_t attribute = H5Aopen(m_id, attributeName.c_str(), H5P_DEFAULT);
  checkHDF5Ptr(attribute, "H5Aopen");
  hid_t attributeType = H5Aget_type(attribute);
  checkHDF5Ptr(attributeType, "H5Aget_type");
  PLI::HDF5::Type returnValue(attributeType);
  checkHDF5Call(H5Aclose(attribute), "H5Aclose");
  return returnValue;
}

void PLI::HDF5::AttributeHandler::copyTo(AttributeHandler dstHandler,
                                         const std::string &srcName,
                                         const std::string &dstName) const {
  if (dstHandler.attributeExists(dstName)) {
    dstHandler.updateAttribute(
        dstName,
        this->getAttribute(srcName, this->attributeType(srcName)).data(),
        this->getAttributeDimensions(srcName), this->attributeType(srcName));
  } else {
    dstHandler.createAttribute(
        dstName,
        this->getAttribute(srcName, this->attributeType(srcName)).data(),
        this->getAttributeDimensions(srcName), this->attributeType(srcName));
  }
}

void PLI::HDF5::AttributeHandler::copyFrom(const AttributeHandler &srcHandler,
                                           const std::string &srcName,
                                           const std::string &dstName) {
  if (srcHandler.attributeExists(srcName)) {
    this->updateAttribute(
        dstName,
        this->getAttribute(srcName, this->attributeType(srcName)).data(),
        this->getAttributeDimensions(srcName), this->attributeType(srcName));
  } else {
    this->createAttribute(
        dstName,
        this->getAttribute(srcName, this->attributeType(srcName)).data(),
        this->getAttributeDimensions(srcName), this->attributeType(srcName));
  }
}

void PLI::HDF5::AttributeHandler::copyAllFrom(
    const AttributeHandler &srcHandler,
    const std::vector<std::string> &exceptions) {
  // Get all attribute names first
  std::vector<std::string> srcHandlerAttribtuteNames =
      srcHandler.attributeNames();

  for (const std::string &attributeName : srcHandlerAttribtuteNames) {
    if (std::find(exceptions.cbegin(), exceptions.cend(), attributeName) !=
        std::cend(exceptions)) {
      this->copyFrom(srcHandler, attributeName, attributeName);
    }
  }
}

void PLI::HDF5::AttributeHandler::copyAllTo(
    AttributeHandler dstHandler, const std::vector<std::string> &exceptions) {
  // Get all attribute names first
  std::vector<std::string> srcHandlerAttribtuteNames = this->attributeNames();

  for (const std::string &attributeName : srcHandlerAttribtuteNames) {
    if (std::find(exceptions.cbegin(), exceptions.cend(), attributeName) !=
        std::cend(exceptions)) {
      this->copyTo(dstHandler, attributeName, attributeName);
    }
  }
}

void PLI::HDF5::AttributeHandler::createAttribute(
    const std::string &attributeName, const void *content,
    const std::vector<hsize_t> &dimensions, const Type dataType) {
  if (attributeExists(attributeName)) {
    throw AttributeExistsException("Attribute with name " + attributeName +
                                   " already exists");
  }
  hid_t spacePtr =
      H5Screate_simple(dimensions.size(), dimensions.data(), nullptr);
  checkHDF5Ptr(spacePtr, "H5Screate_simple");
  hid_t attrPtr = H5Acreate(m_id, attributeName.c_str(), dataType, spacePtr,
                            H5P_DEFAULT, H5P_DEFAULT);
  checkHDF5Ptr(attrPtr, "H5Acreate");
  checkHDF5Call(H5Awrite(attrPtr, dataType, content), "H5Awrite");
}

void PLI::HDF5::AttributeHandler::createAttribute(
    const std::string &attributeName, const void *content,
    const Type dataType) {
  if (attributeExists(attributeName)) {
    throw AttributeExistsException("Attribute with name " + attributeName +
                                   " already exists");
  }
  hid_t attrPtr = H5Acreate(m_id, attributeName.c_str(), dataType, H5P_DEFAULT,
                            H5P_DEFAULT, H5P_DEFAULT);
  checkHDF5Ptr(attrPtr, "H5Acreate");
  checkHDF5Call(H5Awrite(attrPtr, dataType, &content), "H5Awrite");
}

void PLI::HDF5::AttributeHandler::deleteAttribute(
    const std::string &attributeName) {
  checkHDF5Call(H5Adelete(this->m_id, attributeName.c_str()), "H5Adelete");
}

hid_t PLI::HDF5::AttributeHandler::attributePtr(
    const std::string &attributeName) const {
  if (!this->attributeExists(attributeName)) {
    throw AttributeNotFoundException(
        "Could not update attribute because it "
        "does not exist.");
  }
  hid_t attribute = H5Aopen(m_id, attributeName.c_str(), H5P_DEFAULT);
  checkHDF5Ptr(attribute, "H5Aopen");
  return attribute;
}

const std::vector<unsigned char> PLI::HDF5::AttributeHandler::getAttribute(
    const std::string &attributeName, const Type dataType) const {
  if (!this->attributeExists(attributeName)) {
    throw AttributeNotFoundException(
        "Could not update attribute because it "
        "does not exist.");
  }
  hid_t attributeID = H5Aopen(this->m_id, attributeName.c_str(), H5P_DEFAULT);
  checkHDF5Ptr(attributeID, "H5Aopen");
  hid_t attributeType = dataType;

  std::vector<hsize_t> dims = this->getAttributeDimensions(attributeName);
  hsize_t numElements = 1;
  for (size_t i = 0; i < dims.size(); ++i) {
    numElements *= dims[i];
  }
  std::vector<unsigned char> returnContainer;
  returnContainer.resize(numElements * H5Tget_size(attributeType));
  checkHDF5Call(H5Aread(attributeID, attributeType, returnContainer.data()),
                "H5Aread");
  checkHDF5Call(H5Aclose(attributeID), "H5Aclose");

  return returnContainer;
}

const std::vector<hsize_t> PLI::HDF5::AttributeHandler::getAttributeDimensions(
    const std::string &attributeName) const {
  hid_t attributeID = H5Aopen(this->m_id, attributeName.c_str(), H5P_DEFAULT);
  hid_t attributeSpace = H5Aget_space(attributeID);
  int ndims = H5Sget_simple_extent_ndims(attributeSpace);
  std::vector<hsize_t> dims;
  dims.resize(ndims);
  H5Sget_simple_extent_dims(attributeSpace, dims.data(), nullptr);
  checkHDF5Call(H5Sclose(attributeSpace), "H5Sclose");
  checkHDF5Call(H5Aclose(attributeID), "H5Aclose");
  return dims;
}

void PLI::HDF5::AttributeHandler::updateAttribute(
    const std::string &attributeName, const void *content,
    const Type dataType) {
  if (!this->attributeExists(attributeName)) {
    throw AttributeNotFoundException(
        "Could not update attribute because it "
        "does not exist.");
  }
  hid_t attributeID = H5Aopen(this->m_id, attributeName.c_str(), H5P_DEFAULT);
  checkHDF5Ptr(attributeID, "H5Aopen");
  checkHDF5Call(H5Awrite(attributeID, dataType, &content), "H5Awrite");
  checkHDF5Call(H5Aclose(attributeID), "H5Aclose");
}

void PLI::HDF5::AttributeHandler::updateAttribute(
    const std::string &attributeName, const void *content,
    const std::vector<hsize_t> &dimensions, const Type dataType) {
  if (!this->attributeExists(attributeName)) {
    throw AttributeNotFoundException(
        "Could not update attribute because it "
        "does not exist.");
  }
  hid_t attributeID = H5Aopen(this->m_id, attributeName.c_str(), H5P_DEFAULT);
  checkHDF5Ptr(attributeID, "H5Aopen");

  std::vector<hsize_t> attributeSpaceSize =
      this->getAttributeDimensions(attributeName);
  for (size_t i = 0; i < attributeSpaceSize.size(); ++i) {
    if (attributeSpaceSize.at(i) != dimensions.at(i)) {
      throw DimensionMismatchException(
          "Attribute " + attributeName + " has dimensions " +
          std::to_string(attributeSpaceSize.at(i)) +
          " but is being updated with " + std::to_string(dimensions.at(i)));
    }
  }

  checkHDF5Call(H5Awrite(attributeID, dataType, &content), "H5Awrite");
  checkHDF5Call(H5Aclose(attributeID), "H5Aclose");
}
