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

#include <gtest/gtest.h>

#include <algorithm>
#include <vector>

#include "PLIHDF5/chunking.h"

class PLI_HDF5_Chunking : public ::testing::Test {};

TEST_F(PLI_HDF5_Chunking, chunks) {
    {
        std::vector<size_t> chunkDims = {{2048, 2048, 9}};
        std::vector<size_t> dataDims = {
            {chunkDims[0] * 2, chunkDims[1] * 2, 9}};

        auto chunks = PLI::HDF5::chunkedOffsets(dataDims, chunkDims);
        EXPECT_EQ(chunks.size(), 4);
        EXPECT_TRUE(chunks[0].offset == std::vector<size_t>({{0, 0, 0}}));
        EXPECT_TRUE(chunks[1].offset ==
                    std::vector<size_t>({{0, chunkDims[1], 0}}));
        EXPECT_TRUE(chunks[2].offset ==
                    std::vector<size_t>({{chunkDims[0], 0, 0}}));
        EXPECT_TRUE(chunks[3].offset ==
                    std::vector<size_t>({{chunkDims[0], chunkDims[1], 0}}));
        for (auto chunk : chunks) {
            std::cerr << chunk.dim[0] << ", " << chunk.dim[1] << ", "
                      << chunk.dim[2] << std::endl;
            EXPECT_TRUE(chunk.dim == chunkDims);
        }
    }
    {
        std::vector<size_t> chunkDims = {{2048, 2048, 9}};
        std::vector<size_t> dataDims = {
            {chunkDims[0] * 2, chunkDims[1] * 1, 9}};

        auto chunks = PLI::HDF5::chunkedOffsets(dataDims, chunkDims);
        EXPECT_EQ(chunks.size(), 2);
        EXPECT_TRUE(chunks[0].offset == std::vector<size_t>({{0, 0, 0}}));
        for (auto chunk : chunks) {
            EXPECT_TRUE(chunk.dim == chunkDims);
        }
    }
    {
        std::vector<size_t> chunkDims = {{2048, 2048, 9}};
        std::vector<size_t> dataDims = {
            {chunkDims[0] * 1, chunkDims[1] * 2, 9}};

        auto chunks = PLI::HDF5::chunkedOffsets(dataDims, chunkDims);
        EXPECT_EQ(chunks.size(), 2);
        EXPECT_TRUE(chunks[0].offset == std::vector<size_t>({{0, 0, 0}}));
        for (auto chunk : chunks) {
            EXPECT_TRUE(chunk.dim == chunkDims);
        }
    }
}

TEST_F(PLI_HDF5_Chunking, chunk_offset) {
    {
        std::vector<size_t> chunkDims = {{2048, 2048, 9}};
        std::vector<size_t> dataDims = chunkDims;
        std::vector<size_t> offset = {{0, 0, 0}};

        auto chunks = PLI::HDF5::chunkedOffsets(dataDims, chunkDims, offset);
        EXPECT_EQ(chunks.size(), 1);
        EXPECT_TRUE(chunks[0].offset == offset);
        EXPECT_TRUE(chunks[0].dim == chunkDims);
    }
    {
        std::vector<size_t> chunkDims = {{2048, 2048, 9}};
        std::vector<size_t> dataDims = chunkDims;
        std::vector<size_t> offset = {{1, 1, 1}};

        auto chunks = PLI::HDF5::chunkedOffsets(dataDims, chunkDims, offset);
        EXPECT_EQ(chunks.size(), 1);
        EXPECT_TRUE(chunks[0].offset == offset);

        std::vector<size_t> diff(dataDims.size());
        std::transform(dataDims.cbegin(), dataDims.cend(), offset.cbegin(),
                       diff.begin(), std::minus<>{});
        EXPECT_TRUE(chunks[0].dim == diff);
    }
}

TEST_F(PLI_HDF5_Chunking, size_diff) {
    {
        std::vector<size_t> chunkDims = {{2048, 2048, 9}};
        std::vector<size_t> dataDims = chunkDims;
        dataDims[0] += 1;

        auto chunks = PLI::HDF5::chunkedOffsets(dataDims, chunkDims);
        EXPECT_EQ(chunks.size(), 2);
        EXPECT_TRUE(chunks[0].offset == std::vector<size_t>({{0, 0, 0}}));
        EXPECT_TRUE(chunks[0].dim == chunkDims);
        EXPECT_TRUE(chunks[1].offset == std::vector<size_t>({{2048, 0, 0}}));
        EXPECT_TRUE(chunks[1].dim == std::vector<size_t>({{1, 2048, 9}}));
    }
}

int main(int argc, char *argv[]) {
    int result = 0;

    MPI_Init(&argc, &argv);
    ::testing::InitGoogleTest(&argc, argv);
    result = RUN_ALL_TESTS();

    MPI_Finalize();
    return result;
}
