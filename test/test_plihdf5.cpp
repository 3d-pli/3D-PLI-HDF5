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

#include "PLIHDF5/plihdf5.h"

class PLIMTest : public ::testing::Test {
 protected:
  void SetUp() override {
    int32_t rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0 && std::filesystem::exists(_filePath)) {
      bool success = std::filesystem::remove(_filePath);
      ASSERT_TRUE(success);
    }

    std::string temporaryDirectory = testing::TempDir();
    _filePath = temporaryDirectory + "test.h5";
    _file = PLI::HDF5::createFile(_filePath);
    _group = PLI::HDF5::createGroup(_file, "Group");
    _attributeHandler = PLI::HDF5::AttributeHandler(_group);
  }

  void TearDown() override {
    int32_t rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    try {
      _group.close();
      _file.close();
    } catch (PLI::HDF5::Exceptions::HDF5RuntimeException& e) {
    }
    if (rank == 0) {
      bool success = std::filesystem::remove(_filePath);
      ASSERT_TRUE(success);
    }
    MPI_Barrier(MPI_COMM_WORLD);
  }

  std::string _filePath;
  PLI::HDF5::File _file;
  PLI::HDF5::Group _group;
  PLI::HDF5::AttributeHandler _attributeHandler;
};

TEST_F(PLIMTest, AddCreator) {}

TEST_F(PLIMTest, AddID) {}

TEST_F(PLIMTest, AddReference) {}

TEST_F(PLIMTest, AddMultipleReferences) {}

TEST_F(PLIMTest, AddSoftware) {}

TEST_F(PLIMTest, AddSoftwareRevision) {}

TEST_F(PLIMTest, AddSoftwareParameters) {}

TEST_F(PLIMTest, CopyAfterCreation) {
  auto _group2 = PLI::HDF5::createGroup(_file, "test_group2");
  auto _attrHandler2 = PLI::HDF5::AttributeHandler(_group2);

  auto _plim = PLI::PLIM(_attributeHandler);
  _plim.addCreator();
  _plim.addSoftware("test_plihdf5");
  _plim.addSoftwareParameters("None");
  _plim.addSoftwareRevision("0.1");

  auto _plim2 = PLI::PLIM(_attrHandler2);
  _plim2.addCreator();
  _plim2.addSoftware("test_plihdf5");
  _plim2.addSoftwareParameters("None");
  _plim2.addSoftwareRevision("0.1");

  ASSERT_NO_THROW(_attributeHandler.copyAllTo(_attrHandler2));
  ASSERT_NO_THROW(_attrHandler2.copyAllFrom(_attributeHandler));

  _group2.close();
}

int main(int argc, char* argv[]) {
  int result = 0;

  MPI_Init(&argc, &argv);
  ::testing::InitGoogleTest(&argc, argv);
  result = RUN_ALL_TESTS();

  MPI_Finalize();
  return result;
}
