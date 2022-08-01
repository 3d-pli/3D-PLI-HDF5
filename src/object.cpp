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

#include "PLIHDF5/object.h"

PLI::HDF5::Object::Object(hid_t id,
                          const std::optional<MPI_Comm> &communicator) noexcept
    : m_communicator(communicator), m_id(id) {
    checkHDF5Ptr(id, "PLI::HDF5::Object::Object");
    checkHDF5Call(H5Iinc_ref(id), "H5Iinc_ref");
}

PLI::HDF5::Object::Object(const std::optional<MPI_Comm> &communicator) noexcept
    : m_communicator(communicator), m_id(-1) {}

PLI::HDF5::Object::~Object() { close(); }

hid_t PLI::HDF5::Object::id() const noexcept { return m_id; }

std::optional<MPI_Comm> PLI::HDF5::Object::communicator() const noexcept {
    return m_communicator;
}

PLI::HDF5::Object::operator hid_t() const noexcept { return m_id; }

void PLI::HDF5::Object::close() {
    closeFileObjects(H5F_OBJ_LOCAL | ~H5F_OBJ_FILE);
    closeFileObjects(H5F_OBJ_LOCAL | H5F_OBJ_FILE);

    if (H5Iis_valid(m_id)) {
        checkHDF5Call(H5Idec_ref(this->m_id), "H5Idec_ref");
    }
    this->m_id = -1;
}

void PLI::HDF5::Object::closeFileObjects(unsigned int types) {
    if (!H5Iis_valid(m_id)) {
        return;
    }
    H5I_type_t objectType = H5Iget_type(m_id);
    if (objectType != H5I_FILE) {
        return;
    }
    if (H5Iget_ref(m_id) > 1) {
        return;
    }

    size_t objectCount = H5Fget_obj_count(m_id, types);
    std::vector<hid_t> objectIDs(objectCount, -1);
    H5Fget_obj_ids(m_id, types, objectCount, objectIDs.data());

    for (size_t i = 0; i < objectCount; ++i) {
        while (H5Iis_valid(objectIDs[i])) {
            checkHDF5Call(H5Idec_ref(objectIDs[i]), "H5Idec_ref");
        }
    }
}

PLI::HDF5::Folder::Folder(const std::optional<MPI_Comm> &communicator) noexcept
    : Object(communicator) {}
PLI::HDF5::Folder::Folder(const hid_t id,
                          const std::optional<MPI_Comm> &communicator) noexcept
    : Object(id, communicator) {}
