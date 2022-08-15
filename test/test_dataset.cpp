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

#include <cmath>
#include <filesystem>
#include <iostream>

#include "PLIHDF5/dataset.h"
#include "PLIHDF5/file.h"

class PLI_HDF5_Dataset : public ::testing::Test {
  protected:
    void SetUp() override {
        int32_t rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        try {
            _file.close();
        } catch (...) {
            // can occur due to testing failures. leave pointer open and
            // continue.
        }
        if (rank == 0 && std::filesystem::exists(_filePath))
            std::filesystem::remove(_filePath);
        MPI_Barrier(MPI_COMM_WORLD);
        _file = PLI::HDF5::createFile(
            _filePath, PLI::HDF5::File::CreateState::OverrideExisting,
            MPI_COMM_WORLD);
    }

    void TearDown() override {
        int32_t rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        try {
            _file.close();
        } catch (...) {
            // can occur due to testing failures. leave pointer open and
            // continue.
        }
        if (rank == 0 && std::filesystem::exists(_filePath))
            std::filesystem::remove(_filePath);
    }

    const std::vector<size_t> _dims{{128, 128, 4}};
    const std::vector<size_t> _chunk_dims{{128, 128, 4}};
    const std::string _filePath =
        std::filesystem::temp_directory_path() / "test_dataset.h5";
    PLI::HDF5::File _file;
};

TEST_F(PLI_HDF5_Dataset, Constructor) {}
TEST_F(PLI_HDF5_Dataset, Destructor) {}

TEST_F(PLI_HDF5_Dataset, exists) {
    { // checking non existing dset
        EXPECT_FALSE(PLI::HDF5::Dataset::exists(_file, "/Image"));
    }

    { // checking existing dset
        auto dset = _file.createDataset<float>("/Image", _dims, _chunk_dims);
        EXPECT_TRUE(PLI::HDF5::Dataset::exists(_file, "/Image"));
        dset.close();
    }
}

TEST_F(PLI_HDF5_Dataset, close) {
    { // close empty dataset
        auto dset = PLI::HDF5::Dataset();
        EXPECT_NO_THROW(dset.close());
    }

    { // close dataset
        auto dset = _file.createDataset<float>("/Image", _dims, _chunk_dims);
        EXPECT_NO_THROW(dset.close());
    }
}

TEST_F(PLI_HDF5_Dataset, write) {
    { // write closed dset
        auto dset = _file.createDataset<float>("/Image_0", _dims, _chunk_dims);
        dset.close();
        const std::vector<float> data(std::accumulate(
            _dims.begin(), _dims.end(), 1, std::multiplies<int>()));
        const std::vector<size_t> offset{{0, 0, 0}};
        EXPECT_THROW(dset.write(data, offset, _dims),
                     PLI::HDF5::Exceptions::IdentifierNotValidException);
    }

    { // write
        auto dset = _file.createDataset<float>("/Image_1", _dims, _chunk_dims);
        const std::vector<float> data(std::accumulate(
            _dims.begin(), _dims.end(), 1, std::multiplies<std::size_t>()));
        const std::vector<size_t> offset{{0, 0, 0}};
        EXPECT_NO_THROW(dset.write(data, offset, _dims));
        dset.close();
    }

    { // write tile
        auto dset = _file.createDataset<float>("/Image_3", _dims, _chunk_dims);
        const std::vector<size_t> dims{{2, 2, 2}};
        const std::vector<size_t> offset{{1, 1, 1}};
        const std::vector<float> data(std::accumulate(
            dims.begin(), dims.end(), 1, std::multiplies<std::size_t>()));
        EXPECT_NO_THROW(dset.write(data, offset, dims));
        dset.close();
    }

    { // write offset outside of dset
        auto dset = _file.createDataset<float>("/Image_4", _dims, _chunk_dims);
        const std::vector<float> data(std::accumulate(
            _dims.begin(), _dims.end(), 1, std::multiplies<int>()));
        const std::vector<size_t> offset{{1, 1, 1}};
        EXPECT_THROW(dset.write(data, offset, _dims),
                     PLI::HDF5::Exceptions::HDF5RuntimeException);
        dset.close();
    }

    { // Call other write / create methods
        PLI::HDF5::Type type = PLI::HDF5::Type::createType<float>();
        auto dset = _file.createDataset("/Image_5", _dims, _chunk_dims, type);
        const std::vector<float> data(std::accumulate(
            _dims.begin(), _dims.end(), 1, std::multiplies<std::size_t>()));
        const std::vector<size_t> offset{{0, 0, 0}};
        EXPECT_NO_THROW(dset.write(data.data(), offset, _dims, {}, type));
    }

    { // Call other write / create methods
        PLI::HDF5::Type type = PLI::HDF5::Type::createType<float>();
        auto dset = PLI::HDF5::Dataset();
        dset.create(_file, "/Image_6", _dims, _chunk_dims, type);
        const std::vector<float> data(std::accumulate(
            _dims.begin(), _dims.end(), 1, std::multiplies<std::size_t>()));
        const std::vector<size_t> offset{{0, 0, 0}};
        EXPECT_NO_THROW(dset.write<float>(data.data(), offset, _dims));
    }
}

TEST(PLI_HDF5_Dataset_, write_mpi_toggle) {
    int32_t rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    const std::vector<size_t> _dims{{128, 128, 4}};
    const std::vector<size_t> _chunk_dims{{128, 128, 4}};
    const std::string _filePath =
        std::filesystem::temp_directory_path() / "test_dataset.h5";

    if (rank == 0) { // Write with only one rank when using MPI
        try {
            PLI::HDF5::File file = PLI::HDF5::createFile(
                _filePath, PLI::HDF5::File::CreateState::OverrideExisting);
            auto dset =
                file.createDataset<float>("/Image_0", _dims, _chunk_dims);
            const std::vector<float> data(std::accumulate(
                _dims.begin(), _dims.end(), 1, std::multiplies<int>()));
            const std::vector<size_t> offset{{0, 0, 0}};
            EXPECT_NO_THROW(dset.write(data, offset, _dims));

            dset.close();
            file.close();
        } catch (const std::exception &e) {
        }
        if (std::filesystem::exists(_filePath))
            std::filesystem::remove(_filePath);
    }
}

TEST_F(PLI_HDF5_Dataset, type) {
    { // call
        auto dset = _file.createDataset<float>("/Image_1", _dims, _chunk_dims);
        EXPECT_NO_THROW(dset.type());
        dset.close();
    }
}

TEST_F(PLI_HDF5_Dataset, ndims) {
    { // call
        auto dset = _file.createDataset<float>("/Image_1", _dims, _chunk_dims);
        EXPECT_EQ(dset.ndims(), _dims.size());
        dset.close();
    }
}

TEST_F(PLI_HDF5_Dataset, dims) {
    { // call
        auto dset = _file.createDataset<float>("/Image_1", _dims, _chunk_dims);
        EXPECT_EQ(dset.dims(), _dims);
        dset.close();
    }
}

TEST_F(PLI_HDF5_Dataset, id) {
    { // empty dataset
        auto dset = PLI::HDF5::Dataset();
        EXPECT_NO_THROW(dset.id());
        dset.close();
    }
}

TEST_F(PLI_HDF5_Dataset, open) {
    { // create dataset
        auto dset = _file.createDataset<float>("/Image", _dims, _chunk_dims);
        dset.close();

        EXPECT_NO_THROW(dset.open(_file, "/Image"));
        dset.close();
        EXPECT_NO_THROW(dset = _file.openDataset("/Image"));
        dset.close();
    }
}

TEST_F(PLI_HDF5_Dataset, create) {
    { // create dataset
        PLI::HDF5::Dataset dset;
        EXPECT_NO_THROW(
            dset = _file.createDataset<float>("/foo", _dims, _chunk_dims));
        dset.close();
        EXPECT_NO_THROW(dset.create<float>(_file, "/bar", _dims, _chunk_dims));
        dset.close();
    }

    { // recreate dataset
        EXPECT_THROW(_file.createDataset<float>("/foo", _dims, _chunk_dims),
                     PLI::HDF5::Exceptions::DatasetExistsException);
    }

    { // create dataset with grp in path
        EXPECT_THROW(_file.createDataset<float>("/not_existing_grp/dset", _dims,
                                                _chunk_dims),
                     PLI::HDF5::Exceptions::IdentifierNotValidException);
    }

    { // create wrong dims and chunk size
        auto chunk_dims = _dims;
        chunk_dims.pop_back();
        EXPECT_ANY_THROW(
            _file.createDataset<float>("/Image_", _dims, chunk_dims));
    }
}

TEST_F(PLI_HDF5_Dataset, read) {
    // write test dataset
    std::vector<int> data(std::accumulate(_dims.begin(), _dims.end(), 1,
                                          std::multiplies<std::size_t>()));
    std::iota(data.begin(), data.end(), 0);
    const std::vector<size_t> offset{{0, 0, 0}};
    {
        auto dset = _file.createDataset<int>("/Image", _dims, _chunk_dims);
        dset.write(data, offset, _dims);
        dset.close();
    }

    { // read dataset
        auto dset = _file.openDataset("/Image");
        const auto data_in = dset.read<int>(offset, _dims);
        EXPECT_EQ(data_in, data);
        EXPECT_EQ(data_in[data_in.size() - 1], data_in.size() - 1);
        dset.close();
    }

    { // read subset
        auto dset = _file.openDataset("/Image");
        const std::vector<size_t> offset_in{{1, 1, 1}};
        const std::vector<size_t> dims_in{{2, 2, 2}};
        const auto size = std::accumulate(dims_in.begin(), dims_in.end(), 1,
                                          std::multiplies<std::size_t>());
        std::vector<int> data_in;
        std::vector<int> data_comp(size);
        EXPECT_NO_THROW(data_in = dset.read<int>(offset_in, dims_in));
        EXPECT_EQ(data_in.size(), size);

        auto ii = 0u;
        for (auto i = offset_in[0]; i < offset_in[0] + dims_in[0]; i++)
            for (auto j = offset_in[1]; j < offset_in[1] + dims_in[1]; j++)
                for (auto k = offset_in[2]; k < offset_in[2] + dims_in[2];
                     k++) {
                    data_comp[ii++] =
                        data[i * _dims[1] * _dims[2] + j * _dims[2] + k];
                }

        EXPECT_EQ(data_in, data_comp);

        dset.close();
    }
}

TEST_F(PLI_HDF5_Dataset, isChunked) {
    { // create dataset with chunks
        auto dset = _file.createDataset<float>("/Image", _dims, _chunk_dims);
        EXPECT_TRUE(dset.isChunked());
        dset.close();
    }

    { // create dataset without chunks
        auto dset = _file.createDataset<float>("/Image_2", _dims);
        EXPECT_FALSE(dset.isChunked());
        dset.close();
    }
}

TEST_F(PLI_HDF5_Dataset, slice) {
    { // constructor
        EXPECT_NO_THROW(PLI::HDF5::Dataset::Slice());
        EXPECT_NO_THROW(PLI::HDF5::Dataset::Slice(0));
        EXPECT_NO_THROW(PLI::HDF5::Dataset::Slice(0, 0));
        EXPECT_NO_THROW(PLI::HDF5::Dataset::Slice(0, 0, 1));
    }

    { // operator
        EXPECT_EQ(PLI::HDF5::Dataset::Slice(0, 0),
                  PLI::HDF5::Dataset::Slice(0, 0));
        EXPECT_NE(PLI::HDF5::Dataset::Slice(0, 0, 0),
                  PLI::HDF5::Dataset::Slice(1, 0, 0));
        EXPECT_NE(PLI::HDF5::Dataset::Slice(0, 0, 0),
                  PLI::HDF5::Dataset::Slice(0, 1, 0));
        EXPECT_NE(PLI::HDF5::Dataset::Slice(0, 0, 0),
                  PLI::HDF5::Dataset::Slice(0, 0, 1));
    }

    { // toHyperslab
        PLI::HDF5::Dataset::Slice slice;
        PLI::HDF5::Dataset::Hyperslab hyperslab;
        EXPECT_NO_THROW(hyperslab = slice.toHyperslab());
        EXPECT_EQ(hyperslab.size(), 1);
    }
}

TEST_F(PLI_HDF5_Dataset, slices) {
    { // constructor
        EXPECT_NO_THROW(PLI::HDF5::Dataset::Slices());

        EXPECT_NO_THROW(
            PLI::HDF5::Dataset::Slices({PLI::HDF5::Dataset::Slice(0, 0, 0),
                                        PLI::HDF5::Dataset::Slice(0, 0, 0),
                                        PLI::HDF5::Dataset::Slice(0, 0, 0)}));

        std::vector<PLI::HDF5::Dataset::Slice> slices = {
            {PLI::HDF5::Dataset::Slice(0, 0, 0),
             PLI::HDF5::Dataset::Slice(0, 0, 0),
             PLI::HDF5::Dataset::Slice(0, 0, 0)}};
        EXPECT_NO_THROW(PLI::HDF5::Dataset::Slices(slices));
    }

    { // operator
        PLI::HDF5::Dataset::Slices slices0;
        PLI::HDF5::Dataset::Slices slices1{
            {PLI::HDF5::Dataset::Slice(0, 0, 0),
             PLI::HDF5::Dataset::Slice(1, 0, 0),
             PLI::HDF5::Dataset::Slice(2, 0, 0)}};
        EXPECT_EQ(slices0, slices0);
        EXPECT_EQ(slices1, slices1);
        EXPECT_NE(slices0, slices1);
        EXPECT_NE(slices1, slices0);
    }

    { // iterator
        PLI::HDF5::Dataset::Slices slices0;
        EXPECT_EQ(slices0.begin(), slices0.end());
        EXPECT_EQ(slices0.cbegin(), slices0.cend());

        PLI::HDF5::Dataset::Slices slices{{PLI::HDF5::Dataset::Slice(0, 0, 0),
                                           PLI::HDF5::Dataset::Slice(1, 0, 0),
                                           PLI::HDF5::Dataset::Slice(2, 0, 0)}};
        EXPECT_NE(slices.begin(), slices.end());
        EXPECT_NE(slices.cbegin(), slices.cend());
        EXPECT_EQ(std::distance(slices.begin(), slices.end()), slices.size());
    }

    { // vector stuff
        PLI::HDF5::Dataset::Slices slices;
        EXPECT_EQ(slices.size(), 0);
        EXPECT_NO_THROW(slices.push_back(PLI::HDF5::Dataset::Slice(0)));
        EXPECT_EQ(slices.size(), 1);
        EXPECT_NO_THROW(slices.clear());
        EXPECT_EQ(slices.size(), 0);
    }

    { // tohyperslab
        PLI::HDF5::Dataset::Slices slices;
        slices.push_back(PLI::HDF5::Dataset::Slice(0));

        PLI::HDF5::Dataset::Hyperslab hyperslab;
        EXPECT_NO_THROW(hyperslab = slices.toHyperslab());
        EXPECT_EQ(hyperslab.size(), 1);
        EXPECT_EQ(hyperslab.offset(), std::vector<size_t>({0}));
        EXPECT_EQ(hyperslab.count(), std::vector<size_t>({0}));
        EXPECT_EQ(hyperslab.stride(), std::vector<size_t>({1}));
    }
}

TEST_F(PLI_HDF5_Dataset, hyperslab) {
    { // constructor
        EXPECT_NO_THROW(PLI::HDF5::Dataset::Hyperslab());
        EXPECT_NO_THROW(PLI::HDF5::Dataset::Hyperslab(0, 0));
        EXPECT_NO_THROW(PLI::HDF5::Dataset::Hyperslab(0, 0, 1));
        EXPECT_NO_THROW(PLI::HDF5::Dataset::Hyperslab(
            std::vector<size_t>({0}), std::vector<size_t>({0})));
        EXPECT_NO_THROW(PLI::HDF5::Dataset::Hyperslab(
            std::vector<size_t>({0}), std::vector<size_t>({0}),
            std::vector<size_t>({1})));
    }

    { // operator
        auto hyperslab0 = PLI::HDF5::Dataset::Hyperslab();
        auto hyperslab1 = PLI::HDF5::Dataset::Hyperslab(
            std::vector<size_t>({0}), std::vector<size_t>({0}));
        auto hyperslab2 = PLI::HDF5::Dataset::Hyperslab(
            std::vector<size_t>({0}), std::vector<size_t>({0}),
            std::vector<size_t>({1}));
        auto hyperslab3 = PLI::HDF5::Dataset::Hyperslab();
        hyperslab3.push_back(0, 0);

        EXPECT_EQ(hyperslab0, hyperslab0);
        EXPECT_EQ(hyperslab1, hyperslab1);
        EXPECT_EQ(hyperslab2, hyperslab2);
        EXPECT_EQ(hyperslab3, hyperslab3);

        EXPECT_EQ(hyperslab1, hyperslab2);
        EXPECT_EQ(hyperslab1, hyperslab3);

        EXPECT_EQ(hyperslab2, hyperslab1);
        EXPECT_EQ(hyperslab2, hyperslab3);

        EXPECT_EQ(hyperslab3, hyperslab1);
        EXPECT_EQ(hyperslab3, hyperslab2);

        EXPECT_NE(hyperslab0, hyperslab1);
        EXPECT_NE(hyperslab0, hyperslab2);
        EXPECT_NE(hyperslab0, hyperslab3);
        EXPECT_NE(hyperslab1, hyperslab0);
        EXPECT_NE(hyperslab2, hyperslab0);
        EXPECT_NE(hyperslab3, hyperslab0);
    }

    { // vector stuff
        PLI::HDF5::Dataset::Hyperslab hyperslab;
        EXPECT_EQ(hyperslab.size(), 0);
        EXPECT_NO_THROW(hyperslab.push_back(0, 0));
        EXPECT_EQ(hyperslab.size(), 1);
        EXPECT_NO_THROW(hyperslab.push_back(0, 0, 1));
        EXPECT_EQ(hyperslab.size(), 2);
        EXPECT_NO_THROW(hyperslab.clear());
        EXPECT_EQ(hyperslab.size(), 0);
    }

    { // toSlice
        PLI::HDF5::Dataset::Slices slices;

        PLI::HDF5::Dataset::Hyperslab hyperslab;
        hyperslab.push_back(0, 2);
        hyperslab.push_back(1, 3, 2);
        EXPECT_NO_THROW(slices = hyperslab.toSlices());
        EXPECT_EQ(slices.size(), 2);
        EXPECT_EQ(slices[0].start, 0);
        EXPECT_EQ(slices[0].stop, 2 * 1);
        EXPECT_EQ(slices[0].step, 1);
        EXPECT_EQ(slices[1].start, 1);
        EXPECT_EQ(slices[1].stop, 1 + 3 * 2);
        EXPECT_EQ(slices[1].step, 2);
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
