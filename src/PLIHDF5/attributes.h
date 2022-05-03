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
#include <mpi.h>

#ifdef __GNUC__
#include <pwd.h>
#include <unistd.h>
#else
#define NOMINMAX
#include <Windows.h>
#include <lmcons.h>
#endif

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "PLIHDF5/exceptions.h"
#include "PLIHDF5/type.h"

namespace PLI {
namespace HDF5 {
/**
 * @brief Add attributes to an HDF5 file, group, or dataset.
 * This class allows to add, modify or delete attributes stored in an HDF5 file.
 * The class works on the pointers returned by the HDF5 library. The pointers
 * are available from the other classes of this library.
 */
class AttributeHandler {
 public:
  /**
   * @brief Construct a new Attribute Handler object
   * Construct a new Attribute Handler object not associated with any HDF5
   * object. The object cannot be used to store attributes until the
   * setPtr(const hid_t parentPtr) method is called.
   */
  AttributeHandler() noexcept;
  /**
   * @brief Construct a new Attribute Handler object
   * Construct a new Attribute Handler object associated with the HDF5 object
   * identified by the given pointer.
   * @param parentPtr HDF5 object pointer.
   */
  explicit AttributeHandler(const hid_t parentPtr) noexcept;
  /**
   * @brief Set the HDF5 object pointer.
   * @param parentPtr HDF5 object pointer.
   */
  void setPtr(const hid_t parentPtr) noexcept;

  /**
   * @brief Check if an attribute with the given attributeName exists in the
   * HDF5 object pointer.
   * @param attributeName Attribute Name which is searched for.
   * @return true Attribute exsits.
   * @return false Attribute does not exist.
   * @throws PLI::HDF5::Exceptions::IdentifierNotValidException HDF5 object
   * pointer is invalid.
   */
  bool attributeExists(const std::string& attributeName) const;
  /**
   * @brief Return the names of all attributes stored in the HDF5 object
   * pointer. This method searches for all attribute names stored in the HDF5
   * object pointer. For each attribute name a std::string is returned. The
   * order of the names is defined by the HDF5 library. The search parameters
   * are the same as for the HDF5 library function H5Aget_name_by_idx with
   * H5_INDEX_NAME and H5_ITER_INC.
   * @return const std::vector<std::string> Names of all attributes.
   */
  const std::vector<std::string> attributeNames() const;
  /**
   * @brief Get the HDF5 data type of the attribute with the given
   * attributeName.
   * @param attributeName Attribute Name which is searched for.
   * @return PLI::HDF5::Type Template type which contains the hid_t for the HDF5
   * data type.
   */
  PLI::HDF5::Type attributeType(const std::string& attributeName) const;

  template <typename T>
  void createAttribute(const std::string& attributeName,
                       const std::vector<T>& content,
                       const std::vector<hsize_t>& dimensions);
  void createAttribute(const std::string& attributeName, const void* content,
                       const std::vector<hsize_t>& dimensions,
                       const PLI::HDF5::Type dataType);
  template <typename T>
  void createAttribute(const std::string& attributeName, const T& content);
  void createAttribute(const std::string& attributeName, const void* content,
                       const PLI::HDF5::Type dataType);

  void copyTo(PLI::HDF5::AttributeHandler dstHandler,
              const std::string& srcName, const std::string& dstName) const;
  void copyFrom(const PLI::HDF5::AttributeHandler& srcHandler,
                const std::string& srcName, const std::string& dstName);
  void copyAllFrom(
      const PLI::HDF5::AttributeHandler& srcHandler,
      const std::vector<std::string>& exceptions = std::vector<std::string>());
  void copyAllTo(
      PLI::HDF5::AttributeHandler dstHandler,
      const std::vector<std::string>& exceptions = std::vector<std::string>());

  /**
   * @brief Remove attribute with the given attributeName from the HDF5 object
   * pointer. This method tries to delete the attribute with the given
   * attributeName from the HDF5 object pointer. Please note that the behaviour
   * of this method is undefined when the method attributePtr(const std::string&
   * attributeName) is used and is not closed manually.
   * @param attributeName Name of the attribute to be deleted.
   * @throws PLI::HDF5::Exceptions::HDF5RuntimeException HDF5 library function
   * H5Adelete returns an error. This may happen when the attribute does not
   * exist or another error occured.
   */
  void deleteAttribute(const std::string& attributeName);

  /**
   * @brief Get the raw pointer of the attribute with the given attributeName.
   * This method opens the attribute with the given attributeName and returns
   * the raw pointer. Because HDF5 allocates memory and expects the pointer to
   * be closed manually, the pointer has to be closed manually. If not, the
   * memory might get leaked.
   * @param attributeName Attribute Name which is searched for.
   * @return hid_t HDF5 attribute pointer of the attribute with the given
   * attributeName.
   * @throws PLI::HDF5::Exceptions::AttributeNotFoundException Attribute with
   * the given attributeName does not exist.
   * @throws PLI::HDF5::Exceptions::IdentifierNotValidException HDF5 object
   * pointer is invalid.
   */
  hid_t attributePtr(const std::string& attributeName) const;

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
  void updateAttribute(const std::string& attributeName, const void* content,
                       const PLI::HDF5::Type dataType);
  void updateAttribute(const std::string& attributeName, const void* content,
                       const std::vector<hsize_t>& dimensions,
                       const PLI::HDF5::Type dataType);

 private:
  const std::vector<unsigned char> getAttribute(
      const std::string& attributeName, const PLI::HDF5::Type dataType) const;
  hid_t m_id;
};
}  // namespace HDF5
}  // namespace PLI

#include "PLIHDF5/attributes.tpp"
