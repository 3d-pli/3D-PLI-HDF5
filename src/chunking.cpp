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

#include "PLIHDF5/chunking.h"

std::vector<std::tuple<std::vector<hsize_t>, std::vector<hsize_t>>>
PLI::HDF5::chunkedOffsets(
    const std::vector<hsize_t> &dataDims, const std::vector<hsize_t> &chunkDims,
    std::optional<const std::vector<hsize_t>> chunkOffset) {
    std::vector<std::tuple<std::vector<hsize_t>, std::vector<hsize_t>>> result;

    if (dataDims.size() < 2)
        throw PLI::HDF5::Exceptions::DimensionMismatchException(
            "Dimensions not supported yet");

    const auto chunkOffset_ =
        chunkOffset.value_or(std::vector<hsize_t>(dataDims.size(), 0));
    std::vector<hsize_t> offset = chunkOffset_;
    std::vector<hsize_t> count(dataDims.size(), 0);

    bool flag = true;
    while (flag) {
        for (std::size_t i = 0; i < dataDims.size(); i++) {
            count[i] = offset[i] + chunkDims[i];
            if (count[i] >= dataDims[i])
                count[i] = dataDims[i] - offset[i];
        }

        result.push_back(std::make_tuple(offset, count));

        // increment offset at correct dimension, begin with last
        auto i = static_cast<int64_t>(dataDims.size()) - 1;
        while (true) {
            offset[i] += chunkDims[i];
            if (offset[i] >= dataDims[i]) {
                offset[i] = chunkOffset_[i];
                i--;
                if (i < 0) {
                    flag = false;
                    break;
                }
            } else {
                break;
            }
        }
    }
    return result;
}
