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
#include <mpi.h>

#include "PLIHDF5/attributes.h"
#include "PLIHDF5/file.h"

class AttributeHandlerTest : public ::testing::Test {
 protected:
  void SetUp() override {
    std::string temporaryDirectory = testing::TempDir();
    _filePath = temporaryDirectory + "test.h5";
    _file = PLI::HDF5::createFile(_filePath);
    _attributeHandler = PLI::HDF5::AttributeHandler(_file);
  }

  void TearDown() override {
    int32_t rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    _attributeHandler = PLI::HDF5::AttributeHandler(-1);
    _file.close();
    if (rank == 0) {
      bool success = std::filesystem::remove(_filePath);
      ASSERT_TRUE(success);
    }
  }

  std::string _filePath;
  PLI::HDF5::File _file;
  PLI::HDF5::AttributeHandler _attributeHandler;
};

TEST_F(AttributeHandlerTest, SetPointer) {}

TEST_F(AttributeHandlerTest, AttributeExists) {}

TEST_F(AttributeHandlerTest, AttributeNames) {}

TEST_F(AttributeHandlerTest, AttributeType) {}

TEST_F(AttributeHandlerTest, CreateAttribute) {}

TEST_F(AttributeHandlerTest, CopyTo) {}

TEST_F(AttributeHandlerTest, CopyFrom) {}

TEST_F(AttributeHandlerTest, CopyAllTo) {}

TEST_F(AttributeHandlerTest, CopyAllFrom) {}

TEST_F(AttributeHandlerTest, DeleteAttribute) {}

TEST_F(AttributeHandlerTest, GetAttribute) {}

TEST_F(AttributeHandlerTest, GetAttributeDimensions) {}

TEST_F(AttributeHandlerTest, UpdateAttribute) {}

TEST_F(AttributeHandlerTest, Id) {}

int main(int argc, char* argv[]) {
  int result = 0;

  MPI_Init(&argc, &argv);
  ::testing::InitGoogleTest(&argc, argv);
  result = RUN_ALL_TESTS();

  MPI_Finalize();
  return result;
}
