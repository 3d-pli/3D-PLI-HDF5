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
/**
 * @brief Static class to create and manage links
 */
class Link {
 public:
  /**
   * @brief Create a soft link inside a HDF5 file
   * @param parentPtr File or group identifier
   * @param srcPath Path to the source object
   * @param dstPath Destination path of the link
   */
  static void createSoft(const hid_t parentPtr, const std::string &srcPath,
                         const std::string &dstPath);
  /**
   * @brief Create a hard link inside a HDF5 file
   * @param parentPtr File or group identifier
   * @param srcPath Path to the source object
   * @param dstPath Destination path of the link
   */
  static void createHard(const hid_t parentPtr, const std::string &srcPath,
                         const std::string &dstPath);
  /**
   * @brief Create an external link from one HDF5 file to another
   * @param parentPtr File or group identifier
   * @param externalPath Full path to the external file
   * @param srcPath Path to the source object in the external file
   * @param dstPath Destination path of the link
   */
  static void createExternal(const hid_t parentPtr,
                             const std::string &externalPath,
                             const std::string &srcPath,
                             const std::string &dstPath);
  static H5L_info_t getLinkInfo(const hid_t parentPtr, const std::string &path);
  /**
   * @brief Checks if a link exists
   * @param parentPtr File or group identifier
   * @param path Path to the link
   * @return true If it exists
   * @return false If it doesn't exist
   */
  static bool exists(const hid_t parentPtr, const std::string &path);
  /**
   * @brief Checks if a link is a soft link
   * @param parentPtr File or group identifier
   * @param path Path to the link
   * @return true It is a soft link
   * @return false It is not a soft link
   */
  static bool isSoftLink(const hid_t parentPtr, const std::string &path);
  /**
   * @brief Checks if a link is a hard link
   * @param parentPtr File or group identifier
   * @param path Path to the link
   * @return true It is a hard link
   * @return false It is not a hard link
   */
  static bool isHardLink(const hid_t parentPtr, const std::string &path);
  /**
   * @brief Checks if a link is an external link
   * @param parentPtr File or group identifier
   * @param path Path to the link
   * @return true It is an external link
   * @return false It is not an external link
   */
  static bool isExternalLink(const hid_t parentPtr, const std::string &path);
  /**
   * @brief Removes a link from a HDF5 file
   * @param parentPtr File or group identifier
   * @param path Path to the link
   */
  static void deleteLink(const hid_t parentPtr, const std::string &path);
  /**
   * @brief Moves link inside of an HDF5 file.
   * @param parentPtr File or group identifier
   * @param srcPath Source link path
   * @param dstPath Destination link path
   */
  static void moveLink(const hid_t parentPtr, const std::string &srcPath,
                       const std::string &dstPath);
  /**
   * @brief Copies link inside of an HDF5 file.
   * @param parentPtr File or group identifier
   * @param srcPath Source link path
   * @param dstPath Destination link path
   */
  static void copyLink(const hid_t parentPtr, const std::string &srcPath,
                       const std::string &dstPath);
};
}  // namespace HDF5
}  // namespace PLI
