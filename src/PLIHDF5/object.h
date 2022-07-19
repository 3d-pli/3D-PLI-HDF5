#pragma once

#include <hdf5.h>
#include <mpi.h>
#include <optional>

#include "PLIHDF5/exceptions.h"

namespace PLI::HDF5 {
class Object {
  public:
    explicit Object(const std::optional<MPI_Comm> &communicator = {}) noexcept;
    explicit Object(const hid_t id,
                    const std::optional<MPI_Comm> &communicator = {}) noexcept;

    hid_t id() const noexcept;
    std::optional<MPI_Comm> communicator() const noexcept;
    operator hid_t() const noexcept;

  protected:
    std::optional<MPI_Comm> m_communicator;
    hid_t m_id;
};
} // namespace PLI::HDF5