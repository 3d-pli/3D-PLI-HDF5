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

#include <vector>

#include "PLIHDF5/attributes.h"
#include "PLIHDF5/file.h"
#include "PLIHDF5/group.h"

class AttributeHandlerTest : public ::testing::Test {
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

TEST_F(AttributeHandlerTest, SetPointer) {
  auto attrHandler = PLI::HDF5::AttributeHandler();
  EXPECT_NO_THROW(attrHandler.setPtr(_file));
}

TEST_F(AttributeHandlerTest, AttributeExists) {
  ASSERT_FALSE(_attributeHandler.attributeExists("non_existing"));
  _attributeHandler.createAttribute<int>("existing", 1);
  ASSERT_TRUE(_attributeHandler.attributeExists("existing"));
}

TEST_F(AttributeHandlerTest, AttributeNames) {}

TEST_F(AttributeHandlerTest, AttributeType) {}

TEST_F(AttributeHandlerTest, CreateAttribute) {
  // Create one single entry from template
  {
    int32_t simpleVariable = 0;
    ASSERT_NO_THROW(_attributeHandler.createAttribute<int32_t>(
        "simple_single_attribute", simpleVariable));
    auto readAttr =
        _attributeHandler.getAttribute<int32_t>("simple_single_attribute");
    ASSERT_EQ(simpleVariable, readAttr[0]);
  }

  // Create 1D array from template
  {
    std::vector<int32_t> simpleVector;
    simpleVector.resize(10);
    for (size_t i = 0; i < simpleVector.size(); ++i) {
      simpleVector[i] = i;
    }

    ASSERT_NO_THROW(_attributeHandler.createAttribute<int32_t>(
        "simple_vector_attribute", simpleVector, {simpleVector.size()}));

    auto readAttr =
        _attributeHandler.getAttribute<int32_t>("simple_vector_attribute");
    ASSERT_EQ(simpleVector, readAttr);
  }

  // Create one single entry from pointer
  {
    int32_t* testAttr = new int32_t[1];
    testAttr[0] = 1;
    ASSERT_NO_THROW(_attributeHandler.createAttribute(
        "simple_pointer_attribute", testAttr,
        PLI::HDF5::Type::createType<int32_t>()));

    auto readAttr =
        _attributeHandler.getAttribute<int32_t>("simple_pointer_attribute");
    ASSERT_EQ(*testAttr, readAttr[0]);

    delete[] testAttr;
  }

  // Create string attribute
  {
    std::string testString = "This is a test";
    ASSERT_NO_THROW(_attributeHandler.createAttribute<std::string>(
        "simple_string_attribute", testString));

    auto readAttr =
        _attributeHandler.getAttribute<std::string>("simple_string_attribute");
    ASSERT_EQ(testString, readAttr[0]);
  }

  // Create a vector of strings
  {
    std::vector<std::string> stringArray = {
        "This is string 1", "This is string 2", "This is string 3"};
    ASSERT_NO_THROW(_attributeHandler.createAttribute<std::string>(
        "vector_string", stringArray, {stringArray.size()}));

    auto readAttr =
        _attributeHandler.getAttribute<std::string>("vector_string");
    ASSERT_EQ(stringArray, readAttr);
  }
}

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
