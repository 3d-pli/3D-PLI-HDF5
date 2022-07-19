#include "PLIHDF5/object.h"

PLI::HDF5::Object::Object(hid_t id,
                          const std::optional<MPI_Comm> &communicator) noexcept
    : m_communicator(communicator), m_id(id) {
    checkHDF5Ptr(id, "PLI::HDF5::Object::Object");
    checkHDF5Call(H5Iinc_ref(id), "H5Iinc_ref");
}

PLI::HDF5::Object::Object(const std::optional<MPI_Comm> &communicator) noexcept
    : m_communicator(communicator), m_id(-1) {}

hid_t PLI::HDF5::Object::id() const noexcept { return m_id; }

std::optional<MPI_Comm> PLI::HDF5::Object::communicator() const noexcept {
    return m_communicator;
}

PLI::HDF5::Object::operator hid_t() const noexcept { return m_id; }