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
#include "testEnvironment.h"

class PLI_HDF5_Dataset : public ::testing::Test {
 protected:
  void SetUp() override {
    _file = PLI::HDF5::createFile(_filePath);
    std::cerr << _file << std::endl;
  }

  void TearDown() override {
    _file.close();
    if (std::filesystem::exists(_filePath)) std::filesystem::remove(_filePath);
  }
  const std::vector<hsize_t> _dims{{256, 256, 9}};
  const std::vector<hsize_t> _chunk_dims{{256, 256, 9}};
  const std::string _filePath =
      std::filesystem::temp_directory_path() / "test_group.h5";
  const std::string _dsetPath = "/Image";
  PLI::HDF5::File _file;
};

TEST_F(PLI_HDF5_Dataset, Exists) {
  EXPECT_FALSE(PLI::HDF5::Dataset::exists(_file, _dsetPath));

  auto dset =
      PLI::HDF5::createDataset<float>(_file, _dsetPath, _dims, _chunk_dims);
  EXPECT_TRUE(PLI::HDF5::Dataset::exists(_file, _dsetPath));
}

TEST_F(PLI_HDF5_Dataset, Close) {}

TEST_F(PLI_HDF5_Dataset, Write) {}

TEST_F(PLI_HDF5_Dataset, Type) {}

TEST_F(PLI_HDF5_Dataset, NDims) {}

TEST_F(PLI_HDF5_Dataset, Dims) {}

TEST_F(PLI_HDF5_Dataset, ID) {}

TEST_F(PLI_HDF5_Dataset, Open) {}

TEST_F(PLI_HDF5_Dataset, Create) {
  EXPECT_NO_THROW(  // create dataset
      PLI::HDF5::createDataset<float>(_file, _dsetPath, _dims, _chunk_dims););

  EXPECT_THROW(  // recreate dataset
      PLI::HDF5::createDataset<float>(_file, _dsetPath, _dims, _chunk_dims);
      , PLI::HDF5::Exceptions::DatasetExistsException);

  EXPECT_THROW(  // create dataset in not existing group
      PLI::HDF5::createDataset<float>(_file, "/not_existing_grp/dset", _dims,
                                      _chunk_dims);
      , PLI::HDF5::Exceptions::IdentifierNotValidException);
}

int main(int argc, char* argv[]) {
  int result = 0;

  ::testing::InitGoogleTest(&argc, argv);
  ::testing::AddGlobalTestEnvironment(new MPIEnvironment);

  result = RUN_ALL_TESTS();
  return result;
}
