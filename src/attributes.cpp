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

#include <iostream>

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
    throw Exceptions::HDF5RuntimeException(
        "Could not get the number of attributes.");
  }
  std::vector<std::string> attributes;
  attributes.resize(numAttrs);

  for (int i = 0; i < numAttrs; ++i) {
    // Get length of the attribute name
    ssize_t nameLength =
        H5Aget_name_by_idx(this->m_id, ".", H5_INDEX_NAME, H5_ITER_INC,
                           hsize_t(i), nullptr, 0, H5P_DEFAULT) +
        1;
    // Create attribute with the length
    std::unique_ptr<char[]> attrName(new char[nameLength]);
    // Get the actual attribute name
    H5Aget_name_by_idx(this->m_id, ".", H5_INDEX_NAME, H5_ITER_INC, hsize_t(i),
                       attrName.get(), nameLength, H5P_DEFAULT);
    attributes[i] = attrName.get();
    attrName.release();
  }

  return attributes;
}

PLI::HDF5::Type PLI::HDF5::AttributeHandler::attributeType(
    const std::string &attributeName) const {
  if (!this->attributeExists(attributeName)) {
    throw Exceptions::AttributeNotFoundException(
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
  if (this->attributeExists(dstName)) {
    this->updateAttribute(
        dstName,
        srcHandler.getAttribute(srcName, srcHandler.attributeType(srcName))
            .data(),
        srcHandler.getAttributeDimensions(srcName),
        srcHandler.attributeType(srcName));
  } else {
    this->createAttribute(
        dstName,
        srcHandler.getAttribute(srcName, srcHandler.attributeType(srcName))
            .data(),
        srcHandler.getAttributeDimensions(srcName),
        srcHandler.attributeType(srcName));
  }
}

void PLI::HDF5::AttributeHandler::copyAllFrom(
    const AttributeHandler &srcHandler,
    const std::vector<std::string> &exceptions) {
  // Get all attribute names first
  std::vector<std::string> srcHandlerAttribtuteNames =
      srcHandler.attributeNames();

  bool exceptionFound = false;

  for (const std::string &attributeName : srcHandlerAttribtuteNames) {
    if (exceptions.size() == 0 ||
        std::find(exceptions.cbegin(), exceptions.cend(), attributeName) ==
            std::cend(exceptions)) {
      try {
        this->copyFrom(srcHandler, attributeName, attributeName);
      } catch (const Exceptions::AttributeNotFoundException &e) {
        exceptionFound = true;
      } catch (const Exceptions::HDF5RuntimeException &e) {
        exceptionFound = true;
      } catch (const Exceptions::DimensionMismatchException &e) {
        exceptionFound = true;
      }
    }
  }

  if (exceptionFound) {
    throw Exceptions::HDF5RuntimeException(
        "Could not copy all attributes to destination.");
  }
}

void PLI::HDF5::AttributeHandler::copyAllTo(
    AttributeHandler dstHandler, const std::vector<std::string> &exceptions) {
  // Get all attribute names first
  std::vector<std::string> srcHandlerAttribtuteNames = this->attributeNames();

  bool exceptionFound = false;

  for (const std::string &attributeName : srcHandlerAttribtuteNames) {
    if (exceptions.size() == 0 ||
        std::find(exceptions.cbegin(), exceptions.cend(), attributeName) ==
            std::cend(exceptions)) {
      try {
        this->copyTo(dstHandler, attributeName, attributeName);
      } catch (const Exceptions::AttributeNotFoundException &e) {
        exceptionFound = true;
      } catch (const Exceptions::HDF5RuntimeException &e) {
        exceptionFound = true;
      } catch (const Exceptions::DimensionMismatchException &e) {
        exceptionFound = true;
      }
    }
  }

  if (exceptionFound) {
    throw Exceptions::HDF5RuntimeException(
        "Could not copy all attributes to destination.");
  }
}

void PLI::HDF5::AttributeHandler::createAttribute(
    const std::string &attributeName, const void *content,
    const std::vector<hsize_t> &dimensions, const Type dataType) {
  if (attributeExists(attributeName)) {
    throw Exceptions::AttributeExistsException(
        "Attribute with name " + attributeName + " already exists");
  }

  hid_t spacePtr =
      H5Screate_simple(dimensions.size(), dimensions.data(), nullptr);
  checkHDF5Ptr(spacePtr, "H5Screate_simple");
  hid_t attrPtr = H5Acreate(m_id, attributeName.c_str(), dataType, spacePtr,
                            H5P_DEFAULT, H5P_DEFAULT);
  checkHDF5Ptr(attrPtr, "H5Acreate");
  checkHDF5Call(H5Awrite(attrPtr, dataType, content), "H5Awrite");
  checkHDF5Call(H5Aclose(attrPtr));
}

void PLI::HDF5::AttributeHandler::createAttribute(
    const std::string &attributeName, const void *content,
    const Type dataType) {
  createAttribute(attributeName, content, {1}, dataType);
}

template <>
void PLI::HDF5::AttributeHandler::createAttribute<std::string>(
    const std::string &attributeName, const std::string &content) {
  createAttribute<std::string>(attributeName, {content}, {1});
}

template <>
void PLI::HDF5::AttributeHandler::createAttribute<std::string>(
    const std::string &attributeName, const std::vector<std::string> &content,
    const std::vector<hsize_t> &dimensions) {
  // Get largest string element
  size_t maxStringSize = 0;
  for (auto string : content) {
    if (string.size() > maxStringSize) {
      maxStringSize = string.size();
    }
  }

  // Create datatype
  hid_t attrType = H5Tcreate(H5T_STRING, maxStringSize + 1);
  checkHDF5Ptr(attrType, "H5Tcreate");
  checkHDF5Call(H5Tset_strpad(attrType, H5T_STR_NULLTERM), "H5Tset_strpad");
  checkHDF5Call(H5Tset_cset(attrType, H5T_CSET_ASCII), "H5Tset_cset");

  hid_t attrSpace =
      H5Screate_simple(dimensions.size(), dimensions.data(), nullptr);
  checkHDF5Ptr(attrSpace, "H5Screate_simple");

  // Create attribute
  hid_t attrHandle = H5Acreate(m_id, attributeName.c_str(), attrType, attrSpace,
                               H5P_DEFAULT, H5P_DEFAULT);
  checkHDF5Ptr(attrHandle, "H5Acreate");

  // Create a char array containing both strings
  char *strToWrite = new char[content.size() * (maxStringSize + 1)];
  // Set all of strToWrite to \0
  std::fill(strToWrite, strToWrite + content.size() * (maxStringSize + 1),
            '\0');
  for (size_t i = 0; i < content.size(); ++i) {
    // Write the string to the corresponding pointer position and shift the
    // pointer accordingly
    strncpy(strToWrite + i * (maxStringSize + 1), content[i].c_str(),
            content[i].size());
  }

  // Write attribute
  checkHDF5Call(H5Awrite(attrHandle, attrType, strToWrite), "H5Awrite");

  // Delete the allocated memory
  delete[] strToWrite;

  // Close all open references
  checkHDF5Call(H5Aclose(attrHandle), "H5Aclose");
  checkHDF5Call(H5Sclose(attrSpace), "H5Sclose");
  checkHDF5Call(H5Tclose(attrType), "H5Tclose");
}

void PLI::HDF5::AttributeHandler::deleteAttribute(
    const std::string &attributeName) {
  if (!attributeExists(attributeName)) {
    throw Exceptions::AttributeNotFoundException(
        "Attribute with name " + attributeName + " does not exist");
  }
  checkHDF5Call(H5Adelete(this->m_id, attributeName.c_str()), "H5Adelete");
}

hid_t PLI::HDF5::AttributeHandler::attributePtr(
    const std::string &attributeName) const {
  if (!this->attributeExists(attributeName)) {
    throw Exceptions::AttributeNotFoundException(
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
    throw Exceptions::AttributeNotFoundException(
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

template <>
const std::vector<std::string> PLI::HDF5::AttributeHandler::getAttribute(
    const std::string &attributeName) const {
  // Open the attribute, its type and its dataspace
  hid_t attrHandle = H5Aopen(m_id, attributeName.c_str(), H5P_DEFAULT);
  checkHDF5Ptr(attrHandle, "H5Aopen");
  hid_t attrType = H5Aget_type(attrHandle);
  checkHDF5Ptr(attrType, "H5Aget_type");
  hid_t attrSpace = H5Aget_space(attrHandle);
  checkHDF5Ptr(attrSpace, "H5Aget_space");
  size_t attrElemSize = H5Tget_size(H5Aget_type(attrHandle));
  if (attrElemSize == 0) {
    throw PLI::HDF5::Exceptions::HDF5RuntimeException(
        "H5Tget_size returned 0. This should not happen.");
  }
  // Get the dimension count
  int ndims = H5Sget_simple_extent_ndims(attrSpace);
  if (ndims < 0) {
    throw PLI::HDF5::Exceptions::HDF5RuntimeException(
        "H5Sget_simple_extent_ndims failed with value " +
        std::to_string(ndims));
  }

  // Get sizes of dimensions
  std::vector<hsize_t> dimSizes(ndims);
  checkHDF5Call(H5Sget_simple_extent_dims(attrSpace, dimSizes.data(), nullptr),
                "H5Sget_simple_extent_dims");

  // Calculate total number of values
  hsize_t totalElementCount = 1;
  for (size_t i = 0; i < dimSizes.size(); ++i) {
    totalElementCount *= dimSizes[i];
  }

  std::vector<std::string> returnVector;
  returnVector.resize(totalElementCount);

  // Read the attribute
  std::string attributeContent(totalElementCount * attrElemSize, '\0');
  checkHDF5Call(H5Aread(attrHandle, attrType, attributeContent.data()),
                "H5Aread");

  for (hsize_t i = 0; i < totalElementCount; ++i) {
    returnVector[i] = &(attributeContent[i * attrElemSize]);
  }

  // Close all open references
  checkHDF5Call(H5Sclose(attrSpace), "H5Sclose");
  checkHDF5Call(H5Tclose(attrType), "H5Tclose");
  checkHDF5Call(H5Aclose(attrHandle), "H5Aclose");

  return returnVector;
}

const std::vector<hsize_t> PLI::HDF5::AttributeHandler::getAttributeDimensions(
    const std::string &attributeName) const {
  if (!this->attributeExists(attributeName)) {
    throw Exceptions::AttributeNotFoundException(
        "Could not get dimensions of attribute " + attributeName +
        " because it "
        "does not exist.");
  }
  hid_t attributeID = H5Aopen(this->m_id, attributeName.c_str(), H5P_DEFAULT);
  checkHDF5Ptr(attributeID, "H5Aopen");
  hid_t attributeSpace = H5Aget_space(attributeID);
  checkHDF5Ptr(attributeSpace, "H5Aget_space");
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
    throw Exceptions::AttributeNotFoundException(
        "Could not update attribute because it "
        "does not exist.");
  }
  hid_t attributeID = H5Aopen(this->m_id, attributeName.c_str(), H5P_DEFAULT);
  checkHDF5Ptr(attributeID, "H5Aopen");
  checkHDF5Call(H5Awrite(attributeID, dataType, content), "H5Awrite");
  checkHDF5Call(H5Aclose(attributeID), "H5Aclose");
}

void PLI::HDF5::AttributeHandler::updateAttribute(
    const std::string &attributeName, const void *content,
    const std::vector<hsize_t> &dimensions, const Type dataType) {
  if (!this->attributeExists(attributeName)) {
    throw Exceptions::AttributeNotFoundException(
        "Could not update attribute because it "
        "does not exist.");
  }
  hid_t attributeID = H5Aopen(this->m_id, attributeName.c_str(), H5P_DEFAULT);
  checkHDF5Ptr(attributeID, "H5Aopen");

  std::vector<hsize_t> attributeSpaceSize =
      this->getAttributeDimensions(attributeName);

  if (attributeSpaceSize.size() != dimensions.size()) {
    throw Exceptions::DimensionMismatchException(
        "Dimension mismatch in updateAttribute. "
        "Attribute " +
        attributeName + " has " + std::to_string(attributeSpaceSize.size()) +
        " dimensions, but " + std::to_string(dimensions.size()) +
        " were given.");
  }

  for (size_t i = 0; i < attributeSpaceSize.size(); ++i) {
    if (attributeSpaceSize.at(i) != dimensions.at(i)) {
      throw Exceptions::DimensionMismatchException(
          "Attribute " + attributeName + " has dimensions " +
          std::to_string(attributeSpaceSize.at(i)) +
          " but is being updated with " + std::to_string(dimensions.at(i)));
    }
  }

  checkHDF5Call(H5Awrite(attributeID, dataType, content), "H5Awrite");
  checkHDF5Call(H5Aclose(attributeID), "H5Aclose");
}

template <>
void PLI::HDF5::AttributeHandler::updateAttribute(
    const std::string &attributeName, const std::string &content) {
  this->updateAttribute<std::string>(attributeName, {content}, {1});
}

template <>
void PLI::HDF5::AttributeHandler::updateAttribute(
    const std::string &attributeName, const std::vector<std::string> &content,
    const std::vector<hsize_t> &dimensions) {
  if (!this->attributeExists(attributeName)) {
    throw Exceptions::AttributeNotFoundException(
        "Could not update attribute because it "
        "does not exist.");
  }

  this->deleteAttribute(attributeName);
  this->createAttribute<std::string>(attributeName, content, dimensions);
}
