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

#include "PLIHDF5/chunking.h"

class PLI_HDF5_Chunking : public ::testing::Test {};

TEST_F(PLI_HDF5_Chunking, chunks) {
    {
        std::vector<hsize_t> dataDims = {{4048, 4048, 9}};
        std::vector<hsize_t> chunkDims = {{2048, 2048, 9}};

        auto chunks = PLI::HDF5::chunkedOffsets(dataDims, chunkDims);
        EXPECT_EQ(chunks.size(), 4);
    }
    {
        std::vector<hsize_t> dataDims = {{4048, 2048, 9}};
        std::vector<hsize_t> chunkDims = {{2048, 2048, 9}};

        auto chunks = PLI::HDF5::chunkedOffsets(dataDims, chunkDims);
        EXPECT_EQ(chunks.size(), 2);
    }
    {
        std::vector<hsize_t> dataDims = {{2048, 4048, 9}};
        std::vector<hsize_t> chunkDims = {{2048, 2048, 9}};

        auto chunks = PLI::HDF5::chunkedOffsets(dataDims, chunkDims);
        EXPECT_EQ(chunks.size(), 2);
    }
}

TEST_F(PLI_HDF5_Chunking, chunk_offset) {
    {
        std::vector<hsize_t> dataDims = {{4048, 4048, 9}};
        std::vector<hsize_t> chunkDims = {{2048, 2048, 9}};
        std::vector<hsize_t> offset = {{0, 0, 0}};

        auto chunks = PLI::HDF5::chunkedOffsets(dataDims, chunkDims, offset);
        EXPECT_EQ(chunks.size(), 4);
    }

    {
        std::vector<hsize_t> dataDims = {{4048, 4048, 9}};
        std::vector<hsize_t> chunkDims = {{2048, 2048, 9}};
        std::vector<hsize_t> offset = {{2048, 2048, 0}};

        auto chunks = PLI::HDF5::chunkedOffsets(dataDims, chunkDims, offset);
        EXPECT_EQ(chunks.size(), 1);
    }
    {
        std::vector<hsize_t> dataDims = {{4048, 4048, 9}};
        std::vector<hsize_t> chunkDims = {{2048, 2048, 9}};
        std::vector<hsize_t> offset = {{2049, 2048, 0}};

        auto chunks = PLI::HDF5::chunkedOffsets(dataDims, chunkDims, offset);
        EXPECT_EQ(chunks.size(), 1);
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
