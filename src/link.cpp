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

#include "PLIHDF5/link.h"

bool PLI::HDF5::Link::createSoft(const hid_t parentPtr,
                                 const std::string &srcPath,
                                 const std::string &dstPath) {
  return H5Lcreate_soft(srcPath.c_str(), parentPtr, dstPath.c_str(),
                        H5P_DEFAULT, H5P_DEFAULT) >= 0;
}

bool PLI::HDF5::Link::createHard(const hid_t parentPtr,
                                 const std::string &srcPath,
                                 const std::string &dstPath) {
  return H5Lcreate_hard(parentPtr, srcPath.c_str(), parentPtr, dstPath.c_str(),
                        H5P_DEFAULT, H5P_DEFAULT) >= 0;
}

bool PLI::HDF5::Link::createExternal(const hid_t parentPtr,
                                     const std::string &externalPath,
                                     const std::string &srcPath,
                                     const std::string &dstPath) {
  return H5Lcreate_external(externalPath.c_str(), srcPath.c_str(), parentPtr,
                            dstPath.c_str(), H5P_DEFAULT, H5P_DEFAULT) >= 0;
}

bool PLI::HDF5::Link::exists(const hid_t parentPtr, const std::string &path) {
  return H5Lexists(parentPtr, path.c_str(), H5P_DEFAULT) > 0;
}

bool PLI::HDF5::Link::isSoftLink(const hid_t parentPtr,
                                 const std::string &path) {
  H5L_info_t linkInfo;
  H5Lget_info(parentPtr, path.c_str(), &linkInfo, H5P_DEFAULT);
  return linkInfo.type == H5L_TYPE_SOFT;
}

bool PLI::HDF5::Link::isHardLink(const hid_t parentPtr,
                                 const std::string &path) {
  H5L_info_t linkInfo;
  H5Lget_info(parentPtr, path.c_str(), &linkInfo, H5P_DEFAULT);
  return linkInfo.type == H5L_TYPE_HARD;
}

bool PLI::HDF5::Link::isExternalLink(const hid_t parentPtr,
                                     const std::string &path) {
  H5L_info_t linkInfo;
  H5Lget_info(parentPtr, path.c_str(), &linkInfo, H5P_DEFAULT);
  return linkInfo.type == H5L_TYPE_EXTERNAL;
}

bool PLI::HDF5::Link::deleteLink(const hid_t parentPtr,
                                 const std::string &path) {
  return H5Ldelete(parentPtr, path.c_str(), H5P_DEFAULT) >= 0;
}

bool PLI::HDF5::Link::moveLink(const hid_t parentPtr,
                               const std::string &srcPath,
                               const std::string &dstPath) {
  return H5Lmove(parentPtr, srcPath.c_str(), parentPtr, dstPath.c_str(),
                 H5P_DEFAULT, H5P_DEFAULT) >= 0;
}

bool PLI::HDF5::Link::copyLink(const hid_t parentPtr,
                               const std::string &srcPath,
                               const std::string &dstPath) {
  return H5Lcopy(parentPtr, srcPath.c_str(), parentPtr, dstPath.c_str(),
                 H5P_DEFAULT, H5P_DEFAULT) >= 0;
}
