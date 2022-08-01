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

#include <optional>
#include <vector>

#include "PLIHDF5/exceptions.h"
#include "PLIHDF5/type.h"

namespace PLI {
namespace HDF5 {

struct ChunkParam {
    ChunkParam(const std::vector<size_t> &offset_,
               const std::vector<size_t> &dim_)
        : offset(offset_), dim(dim_){};

    std::vector<size_t> offset;
    std::vector<size_t> dim;
};

std::vector<ChunkParam>
chunkedOffsets(const std::vector<size_t> &dataDims,
               const std::vector<size_t> &chunkDims,
               std::optional<const std::vector<size_t>> chunkOffset = {});
} // namespace HDF5
} // namespace PLI
