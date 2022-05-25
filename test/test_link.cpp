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

#include "PLIHDF5/file.h"
#include "PLIHDF5/group.h"
#include "PLIHDF5/link.h"

class PLI_HDF5_Link : public ::testing::Test {
 protected:
  void SetUp() override {
    int32_t rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    try {
      _parentFile.close();
      _destinationFile.close();
    } catch (...) {
      // can occur due to testing failures. leave pointer open and continue.
    }
    if (rank == 0 && std::filesystem::exists(_parentPath))
      std::filesystem::remove(_parentPath);
    MPI_Barrier(MPI_COMM_WORLD);
    // Create main file for tests
    _parentFile = PLI::HDF5::createFile(_parentPath);
    // Create a dummy group to test links
    auto _group = PLI::HDF5::createGroup(_parentFile, _groupLocation);
    _group.close();
    // Create a dummy file for external links
    if (rank == 0 && std::filesystem::exists(_destinationPath))
      std::filesystem::remove(_destinationPath);
    MPI_Barrier(MPI_COMM_WORLD);
    _destinationFile = PLI::HDF5::createFile(_destinationPath);
  }

  void TearDown() override {
    int32_t rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    try {
      _parentFile.close();
      _destinationFile.close();
    } catch (...) {
      // can occur due to testing failures. leave pointer open and continue.
    }
    if (rank == 0 && std::filesystem::exists(_parentPath))
      std::filesystem::remove(_parentPath);
    if (rank == 0 && std::filesystem::exists(_destinationPath))
      std::filesystem::remove(_destinationPath);
  }

  PLI::HDF5::File _parentFile;
  PLI::HDF5::File _destinationFile;
  const std::string _groupLocation = "linkFrom";
  const std::string _parentPath =
      std::filesystem::temp_directory_path() / "test_link_parent.h5";
  const std::string _destinationPath =
      std::filesystem::temp_directory_path() / "test_link_destination.h5";
};

TEST_F(PLI_HDF5_Link, CreateSoft) {
  PLI::HDF5::Link::createSoft(_parentFile, _groupLocation, "link");
  ASSERT_GT(H5Lexists(_parentFile, "link", H5P_DEFAULT), 0);
  ASSERT_TRUE(PLI::HDF5::Link::isSoftLink(_parentFile, "link"));
}

TEST_F(PLI_HDF5_Link, CreateHard) {
  PLI::HDF5::Link::createHard(_parentFile, _groupLocation, "link");
  ASSERT_GT(H5Lexists(_parentFile, "link", H5P_DEFAULT), 0);
  ASSERT_TRUE(PLI::HDF5::Link::isHardLink(_parentFile, "link"));
}

TEST_F(PLI_HDF5_Link, CreateExternal) {
  PLI::HDF5::Link::createExternal(_destinationFile, _parentPath, _groupLocation,
                                  "link");
  ASSERT_GT(H5Lexists(_destinationFile, "link", H5P_DEFAULT), 0);
  ASSERT_TRUE(PLI::HDF5::Link::isExternalLink(_destinationFile, "link"));
}

TEST_F(PLI_HDF5_Link, Exists) {
  ASSERT_FALSE(PLI::HDF5::Link::exists(_parentFile, "link"));
  PLI::HDF5::Link::createSoft(_parentFile, _groupLocation, "link");
  ASSERT_TRUE(PLI::HDF5::Link::exists(_parentFile, "link"));
}

TEST_F(PLI_HDF5_Link, IsSoftLink) {
  EXPECT_THROW(PLI::HDF5::Link::isSoftLink(_parentFile, "non_existing"),
               PLI::HDF5::Exceptions::HDF5RuntimeException);
  PLI::HDF5::Link::createHard(_parentFile, _groupLocation, "hard_link");
  ASSERT_FALSE(PLI::HDF5::Link::isSoftLink(_parentFile, "hard_link"));
  PLI::HDF5::Link::createSoft(_parentFile, _groupLocation, "soft_link");
  ASSERT_TRUE(PLI::HDF5::Link::isSoftLink(_parentFile, "soft_link"));
  ASSERT_FALSE(PLI::HDF5::Link::isSoftLink(_parentFile, _groupLocation));
}

TEST_F(PLI_HDF5_Link, IsHardLink) {
  EXPECT_THROW(PLI::HDF5::Link::isHardLink(_parentFile, "non_existing"),
               PLI::HDF5::Exceptions::HDF5RuntimeException);
  PLI::HDF5::Link::createHard(_parentFile, _groupLocation, "hard_link");
  ASSERT_TRUE(PLI::HDF5::Link::isHardLink(_parentFile, "hard_link"));
  PLI::HDF5::Link::createSoft(_parentFile, _groupLocation, "soft_link");
  ASSERT_FALSE(PLI::HDF5::Link::isHardLink(_parentFile, "soft_link"));
  ASSERT_TRUE(PLI::HDF5::Link::isHardLink(_parentFile, _groupLocation));
}

TEST_F(PLI_HDF5_Link, IsExternalLink) {
  EXPECT_THROW(PLI::HDF5::Link::isExternalLink(_parentFile, "non_existing"),
               PLI::HDF5::Exceptions::HDF5RuntimeException);
  PLI::HDF5::Link::createHard(_parentFile, _groupLocation, "hard_link");
  ASSERT_FALSE(PLI::HDF5::Link::isExternalLink(_parentFile, "hard_link"));
  PLI::HDF5::Link::createExternal(_destinationFile, _parentPath, _groupLocation,
                                  "external_link");
  ASSERT_TRUE(
      PLI::HDF5::Link::isExternalLink(_destinationFile, "external_link"));
  ASSERT_FALSE(PLI::HDF5::Link::isExternalLink(_parentFile, _groupLocation));
}

TEST_F(PLI_HDF5_Link, DeleteLink) {
  PLI::HDF5::Link::createSoft(_parentFile, _groupLocation, "link");
  ASSERT_GT(H5Lexists(_parentFile, "link", H5P_DEFAULT), 0);
  PLI::HDF5::Link::deleteLink(_parentFile, "link");
  ASSERT_LE(H5Lexists(_parentFile, "link", H5P_DEFAULT), 0);
}

TEST_F(PLI_HDF5_Link, MoveLink) {
  PLI::HDF5::Link::createSoft(_parentFile, _groupLocation, "link");
  ASSERT_GT(H5Lexists(_parentFile, "link", H5P_DEFAULT), 0);
  PLI::HDF5::Link::moveLink(_parentFile, "link", "movedLink");
  ASSERT_LE(H5Lexists(_parentFile, "link", H5P_DEFAULT), 0);
  ASSERT_GT(H5Lexists(_parentFile, "movedLink", H5P_DEFAULT), 0);
}

TEST_F(PLI_HDF5_Link, CopyLink) {
  PLI::HDF5::Link::createSoft(_parentFile, _groupLocation, "link");
  ASSERT_GT(H5Lexists(_parentFile, "link", H5P_DEFAULT), 0);
  PLI::HDF5::Link::copyLink(_parentFile, "link", "copiedLink");
  ASSERT_GT(H5Lexists(_parentFile, "link", H5P_DEFAULT), 0);
  ASSERT_GT(H5Lexists(_parentFile, "copiedLink", H5P_DEFAULT), 0);
}

int main(int argc, char* argv[]) {
  int result = 0;

  MPI_Init(&argc, &argv);
  ::testing::InitGoogleTest(&argc, argv);
  result = RUN_ALL_TESTS();

  MPI_Finalize();
  return result;
}
