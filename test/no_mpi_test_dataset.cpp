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

#include <filesystem>
#include <numeric>

#include "PLIHDF5/dataset.h"
#include "PLIHDF5/file.h"

class PLI_HDF5_Dataset_Non_MPI : public ::testing::Test {
  protected:
    void SetUp() override {
        try {
            _file.close();
        } catch (...) {
            // can occur due to testing failures. leave pointer open and
            // continue.
        }
        if (std::filesystem::exists(_filePath))
            std::filesystem::remove(_filePath);
        _file = PLI::HDF5::createFile(
            _filePath, PLI::HDF5::File::CreateState::OverrideExisting);
    }

    void TearDown() override {
        try {
            _file.close();
        } catch (...) {
            // can occur due to testing failures. leave pointer open and
            // continue.
        }
        if (std::filesystem::exists(_filePath))
            std::filesystem::remove(_filePath);
    }

    const std::vector<size_t> _dims{{128, 128, 4}};
    const std::vector<size_t> _chunk_dims{{128, 128, 4}};
    const std::string _filePath =
        std::filesystem::temp_directory_path() / "test_dataset.h5";
    PLI::HDF5::File _file;
};

TEST_F(PLI_HDF5_Dataset_Non_MPI, Constructor) {}
TEST_F(PLI_HDF5_Dataset_Non_MPI, Destructor) {}

TEST_F(PLI_HDF5_Dataset_Non_MPI, exists) {
    { // checking non existing dset
        EXPECT_FALSE(PLI::HDF5::Dataset::exists(_file, "/Image"));
    }

    { // checking existing dset
        auto dset = PLI::HDF5::createDataset<float>(_file, "/Image", _dims,
                                                    _chunk_dims);
        EXPECT_TRUE(PLI::HDF5::Dataset::exists(_file, "/Image"));
        dset.close();
    }
}

TEST_F(PLI_HDF5_Dataset_Non_MPI, close) {
    { // close empty dataset
        auto dset = PLI::HDF5::Dataset();
        EXPECT_NO_THROW(dset.close());
    }

    { // close dataset
        auto dset = PLI::HDF5::createDataset<float>(_file, "/Image", _dims,
                                                    _chunk_dims);
        EXPECT_NO_THROW(dset.close());
    }
}

TEST_F(PLI_HDF5_Dataset_Non_MPI, write) {
    { // write closed dset
        auto dset = PLI::HDF5::createDataset<float>(_file, "/Image_0", _dims,
                                                    _chunk_dims);
        dset.close();
        const std::vector<float> data(std::accumulate(
            _dims.begin(), _dims.end(), 1, std::multiplies<int>()));
        const std::vector<size_t> offset{{0, 0, 0}};
        EXPECT_THROW(dset.write(data, offset, _dims),
                     PLI::HDF5::Exceptions::IdentifierNotValidException);
    }

    { // write
        auto dset = PLI::HDF5::createDataset<float>(_file, "/Image_1", _dims,
                                                    _chunk_dims);
        const std::vector<float> data(std::accumulate(
            _dims.begin(), _dims.end(), 1, std::multiplies<std::size_t>()));
        const std::vector<size_t> offset{{0, 0, 0}};
        EXPECT_NO_THROW(dset.write(data, offset, _dims));
        dset.close();
    }

    { // write different type
        auto dset = PLI::HDF5::createDataset<float>(_file, "/Image_2", _dims,
                                                    _chunk_dims);
        const std::vector<double> data(std::accumulate(
            _dims.begin(), _dims.end(), 1, std::multiplies<std::size_t>()));
        const std::vector<size_t> offset{{0, 0, 0}};
        EXPECT_NO_THROW(dset.write(data, offset, _dims));
        dset.close();
    }

    { // write tile
        auto dset = PLI::HDF5::createDataset<float>(_file, "/Image_3", _dims,
                                                    _chunk_dims);
        const std::vector<size_t> dims{{2, 2, 2}};
        const std::vector<size_t> offset{{1, 1, 1}};
        const std::vector<float> data(std::accumulate(
            dims.begin(), dims.end(), 1, std::multiplies<std::size_t>()));
        EXPECT_NO_THROW(dset.write(data, offset, dims));
        dset.close();
    }

    { // write offset outside of dset
        auto dset = PLI::HDF5::createDataset<float>(_file, "/Image_4", _dims,
                                                    _chunk_dims);
        const std::vector<float> data(std::accumulate(
            _dims.begin(), _dims.end(), 1, std::multiplies<int>()));
        const std::vector<size_t> offset{{1, 1, 1}};
        EXPECT_THROW(dset.write(data, offset, _dims),
                     PLI::HDF5::Exceptions::HDF5RuntimeException);
        dset.close();
    }

    { // Call other write / create methods
        PLI::HDF5::Type type = PLI::HDF5::Type::createType<float>();
        auto dset = PLI::HDF5::createDataset(_file, "/Image_5", _dims,
                                             _chunk_dims, type);
        const std::vector<float> data(std::accumulate(
            _dims.begin(), _dims.end(), 1, std::multiplies<std::size_t>()));
        const std::vector<size_t> offset{{0, 0, 0}};
        EXPECT_NO_THROW(dset.write(data.data(), offset, _dims, type));
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

TEST_F(PLI_HDF5_Dataset_Non_MPI, type) {
    { // call
        auto dset = PLI::HDF5::createDataset<float>(_file, "/Image_1", _dims,
                                                    _chunk_dims);
        EXPECT_NO_THROW(dset.type());
        dset.close();
    }
}

TEST_F(PLI_HDF5_Dataset_Non_MPI, ndims) {
    { // call
        auto dset = PLI::HDF5::createDataset<float>(_file, "/Image_1", _dims,
                                                    _chunk_dims);
        EXPECT_TRUE(dset.ndims() == _dims.size());
        dset.close();
    }
}

TEST_F(PLI_HDF5_Dataset_Non_MPI, dims) {
    { // call
        auto dset = PLI::HDF5::createDataset<float>(_file, "/Image_1", _dims,
                                                    _chunk_dims);
        EXPECT_TRUE(dset.dims() == _dims);
        dset.close();
    }
}

TEST_F(PLI_HDF5_Dataset_Non_MPI, id) {
    { // empty dataset
        auto dset = PLI::HDF5::Dataset();
        EXPECT_NO_THROW(dset.id());
        dset.close();
    }
}

TEST_F(PLI_HDF5_Dataset_Non_MPI, open) {
    { // create dataset
        auto dset = PLI::HDF5::createDataset<float>(_file, "/Image", _dims,
                                                    _chunk_dims);
        dset.close();

        EXPECT_NO_THROW(dset.open(_file, "/Image"));
        dset.close();
        EXPECT_NO_THROW(dset = PLI::HDF5::openDataset(_file, "/Image"));
        dset.close();
    }
}

TEST_F(PLI_HDF5_Dataset_Non_MPI, create) {
    { // create dataset
        PLI::HDF5::Dataset dset;
        EXPECT_NO_THROW(dset = PLI::HDF5::createDataset<float>(
                            _file, "/foo", _dims, _chunk_dims));
        dset.close();
        EXPECT_NO_THROW(dset.create<float>(_file, "/bar", _dims, _chunk_dims));
        dset.close();
    }

    { // recreate dataset
        EXPECT_THROW(
            PLI::HDF5::createDataset<float>(_file, "/foo", _dims, _chunk_dims),
            PLI::HDF5::Exceptions::DatasetExistsException);
    }

    { // create dataset with grp in path
        EXPECT_THROW(PLI::HDF5::createDataset<float>(
                         _file, "/not_existing_grp/dset", _dims, _chunk_dims),
                     PLI::HDF5::Exceptions::IdentifierNotValidException);
    }

    { // create wrong dims and chunk size
        auto chunk_dims = _dims;
        chunk_dims.pop_back();
        EXPECT_ANY_THROW(PLI::HDF5::createDataset<float>(_file, "/Image_",
                                                         _dims, chunk_dims));
    }
}

TEST_F(PLI_HDF5_Dataset_Non_MPI, readFullDataset) {}

TEST_F(PLI_HDF5_Dataset_Non_MPI, read) {
    // write test dataset
    std::vector<int> data(std::accumulate(_dims.begin(), _dims.end(), 1,
                                          std::multiplies<std::size_t>()));
    std::iota(data.begin(), data.end(), 0);
    const std::vector<size_t> offset{{0, 0, 0}};
    {
        auto dset =
            PLI::HDF5::createDataset<int>(_file, "/Image", _dims, _chunk_dims);
        dset.write(data, offset, _dims);
        dset.close();
    }

    { // read dataset
        auto dset = PLI::HDF5::openDataset(_file, "/Image");
        const auto data_in = dset.read<int>(offset, _dims);
        EXPECT_TRUE(data_in == data);
        EXPECT_TRUE(data_in[data_in.size() - 1] == data_in.size() - 1);
        dset.close();
    }

    { // read subset
        auto dset = PLI::HDF5::openDataset(_file, "/Image");
        const std::vector<size_t> offset_in{{1, 1, 1}};
        const std::vector<size_t> dims_in{{2, 2, 2}};
        const auto size = std::accumulate(dims_in.begin(), dims_in.end(), 1,
                                          std::multiplies<std::size_t>());
        std::vector<int> data_in;
        std::vector<int> data_comp(size);
        EXPECT_NO_THROW(data_in = dset.read<int>(offset_in, dims_in));
        EXPECT_TRUE(data_in.size() == size);

        auto ii = 0u;
        for (auto i = offset_in[0]; i < offset_in[0] + dims_in[0]; i++)
            for (auto j = offset_in[1]; j < offset_in[1] + dims_in[1]; j++)
                for (auto k = offset_in[2]; k < offset_in[2] + dims_in[2];
                     k++) {
                    data_comp[ii++] =
                        data[i * _dims[1] * _dims[2] + j * _dims[2] + k];
                }

        EXPECT_TRUE(data_in == data_comp);

        dset.close();
    }
}

// TEST_F(PLI_HDF5_Dataset_Non_MPI, hid_t) {} // operator??

int main(int argc, char *argv[]) {
    int result = 0;

    ::testing::InitGoogleTest(&argc, argv);
    result = RUN_ALL_TESTS();

    return result;
}
