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

#include "PLIHDF5/exceptions.h"

H5L_info_t PLI::HDF5::Link::getLinkInfo(const Folder &parentPtr,
                                        const std::string &path) {
    H5L_info_t linkInfo;
    checkHDF5Ptr(parentPtr, "getLinkInfo");
    checkHDF5Call(H5Lget_info(parentPtr, path.c_str(), &linkInfo, H5P_DEFAULT),
                  "H5Lget_info");
    return linkInfo;
}

void PLI::HDF5::Link::createSoft(const Folder &parentPtr,
                                 const std::string &srcPath,
                                 const std::string &dstPath) {
    checkHDF5Ptr(parentPtr, "createSoft");
    checkHDF5Call(H5Lcreate_soft(srcPath.c_str(), parentPtr, dstPath.c_str(),
                                 H5P_DEFAULT, H5P_DEFAULT),
                  "H5Lcreate_soft");
}

void PLI::HDF5::Link::createHard(const Folder &parentPtr,
                                 const std::string &srcPath,
                                 const std::string &dstPath) {
    checkHDF5Ptr(parentPtr, "createHard");
    checkHDF5Call(H5Lcreate_hard(parentPtr, srcPath.c_str(), parentPtr,
                                 dstPath.c_str(), H5P_DEFAULT, H5P_DEFAULT),
                  "H5Lcreate_hard");
}

void PLI::HDF5::Link::createExternal(const Folder &parentPtr,
                                     const std::string &externalPath,
                                     const std::string &srcPath,
                                     const std::string &dstPath) {
    checkHDF5Ptr(parentPtr, "createExternal");
    checkHDF5Call(H5Lcreate_external(externalPath.c_str(), srcPath.c_str(),
                                     parentPtr, dstPath.c_str(), H5P_DEFAULT,
                                     H5P_DEFAULT),
                  "H5Lcreate_external");
}

bool PLI::HDF5::Link::exists(const Folder &parentPtr, const std::string &path) {
    checkHDF5Ptr(parentPtr, "exists");
    return H5Lexists(parentPtr, path.c_str(), H5P_DEFAULT) > 0;
}

bool PLI::HDF5::Link::isSoftLink(const Folder &parentPtr,
                                 const std::string &path) {
    return getLinkInfo(parentPtr, path).type == H5L_TYPE_SOFT;
}

bool PLI::HDF5::Link::isHardLink(const Folder &parentPtr,
                                 const std::string &path) {
    return getLinkInfo(parentPtr, path).type == H5L_TYPE_HARD;
}

bool PLI::HDF5::Link::isExternalLink(const Folder &parentPtr,
                                     const std::string &path) {
    return getLinkInfo(parentPtr, path).type == H5L_TYPE_EXTERNAL;
}

void PLI::HDF5::Link::deleteLink(const Folder &parentPtr,
                                 const std::string &path) {
    checkHDF5Ptr(parentPtr, "deleteLink");
    checkHDF5Call(H5Ldelete(parentPtr, path.c_str(), H5P_DEFAULT), "H5Ldelete");
}

void PLI::HDF5::Link::moveLink(const Folder &parentPtr,
                               const std::string &srcPath,
                               const std::string &dstPath) {
    checkHDF5Ptr(parentPtr, "moveLink");
    checkHDF5Call(H5Lmove(parentPtr, srcPath.c_str(), parentPtr,
                          dstPath.c_str(), H5P_DEFAULT, H5P_DEFAULT),
                  "H5Lmove");
}

void PLI::HDF5::Link::copyLink(const Folder &parentPtr,
                               const std::string &srcPath,
                               const std::string &dstPath) {
    checkHDF5Ptr(parentPtr, "copyLink");
    checkHDF5Call(H5Lcopy(parentPtr, srcPath.c_str(), parentPtr,
                          dstPath.c_str(), H5P_DEFAULT, H5P_DEFAULT),
                  "H5Lcopy");
}
