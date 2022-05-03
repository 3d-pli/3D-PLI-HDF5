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
 * @brief HDF5 Group wrapper class.
 * Create and / or open an HDF5 group.
 */
class Group {
 public:
  /**
   * @brief Construct a new Group object
   * Create an empty group object. Calling other methods on this object will
   * fail because no pointer to an HDF5 object is set. To open a group, use
   * PLI::HDF5::Group::open(hid_t parentPtr, const std::string& groupName). To
   * create a grouop, use PLI::HDF5::File::create(hid_t parentPtr, const
   * std::string& groupName).
   */
  Group() noexcept;
  /**
   * @brief Construct a new Group object
   * Construct a new group object by using the given HDF5 pointer of another
   * group object.
   * @param otherGroup Other group object.
   */
  explicit Group(const hid_t groupPtr) noexcept;
  /**
   * @brief
   *
   * @param parentPtr
   * @param groupName
   * @return PLI::HDF5::Group
   */
  static PLI::HDF5::Group open(hid_t parentPtr, const std::string& groupName);
  /**
   * @brief
   *
   * @param parentPtr
   * @param groupName
   * @return PLI::HDF5::Group
   */
  static PLI::HDF5::Group create(hid_t parentPtr, const std::string& groupName);
  /**
   * @brief
   *
   * @param parentPtr
   * @param groupName
   * @return true
   * @return false
   */
  static bool exists(hid_t parentPtr, const std::string& groupName);

  /**
   * @brief
   *
   */
  void close();
  /**
   * @brief
   *
   * @return hid_t
   */
  hid_t id() const noexcept;
  /**
   * @brief
   *
   * @return hid_t
   */
  operator hid_t() const noexcept;

 private:
  hid_t m_id;
};
}  // namespace HDF5
}  // namespace PLI
