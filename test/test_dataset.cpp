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

#include "PLIHDF5/dataset.h"
#include "PLIHDF5/file.h"
#include "testEnvironment.h"

class DatasetFixtureTest : public ::testing::Test {
 protected:
  void SetUp() override {
    _filePath = "test_dataset.h5";
    _copyFilePath = "test_dataset.h5.bak";
    // std::filesystem::copy_file(
    //     _filePath, _copyFilePath,
    //     std::filesystem::copy_options::overwrite_existing);
    // _file = PLI::HDF5::File::open(_filePath, H5F_ACC_RDWR);
    // _dataset = PLI::HDF5::Dataset::open(_file, "test");
  }

  void TearDown() override {
    _dataset.close();
    _file.close();
    // std::filesystem::copy_file(
    //     _copyFilePath, _filePath,
    //     std::filesystem::copy_options::overwrite_existing);
  }

  std::string _filePath, _copyFilePath;
  PLI::HDF5::File _file;
  PLI::HDF5::Dataset _dataset;
};

TEST_F(DatasetFixtureTest, Exists) {}

TEST_F(DatasetFixtureTest, Close) {}

TEST_F(DatasetFixtureTest, Write) {}

TEST_F(DatasetFixtureTest, Type) {}

TEST_F(DatasetFixtureTest, NDims) {}

TEST_F(DatasetFixtureTest, Dims) {}

TEST_F(DatasetFixtureTest, ID) {}

TEST(DatasetTest, Open) {}

TEST(DatasetTest, Create) {}

int main(int argc, char* argv[]) {
  int result = 0;

  ::testing::InitGoogleTest(&argc, argv);
  ::testing::AddGlobalTestEnvironment(new MPIEnvironment);

  result = RUN_ALL_TESTS();
  return result;
}
