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

#include "PLIHDF5/file.h"

class PLI_HDF5_File : public ::testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {
    _file.close();
    if (std::filesystem::exists(_filePath)) std::filesystem::remove(_filePath);
  }

  const std::string _filePath =
      std::filesystem::temp_directory_path() / "test_file.h5";
  PLI::HDF5::File _file;
};

TEST_F(PLI_HDF5_File, Constructor) {
  EXPECT_NO_THROW({ auto h5f = PLI::HDF5::File(); });

  EXPECT_NO_THROW({
    auto h5f_0 = PLI::HDF5::File();
    auto h5f_1 = PLI::HDF5::File(h5f_0);
  });

  // TODO: File(const hid_t filePtr, const hid_t faplID);
}

TEST_F(PLI_HDF5_File, Destructor) {
  EXPECT_NO_THROW({  // invoke deconstroctor
    auto h5f = PLI::HDF5::File();
  });
}

TEST_F(PLI_HDF5_File, Create) {
  {  // create file
    auto h5f = PLI::HDF5::File();
    h5f.create(_filePath);
  }

  {  // create existing file
    auto h5f = PLI::HDF5::File();
    EXPECT_THROW(h5f.create(_filePath),
                 PLI::HDF5::Exceptions::FileExistsException);
  }
}

TEST_F(PLI_HDF5_File, Open) {
  {  // create dummy file
    auto h5f = PLI::HDF5::File();
    h5f.create(_filePath);
    h5f.close();
  }

  {  // open dummy file
    auto h5f = PLI::HDF5::File();
    EXPECT_NO_THROW(h5f.open(_filePath, PLI::HDF5::File::OpenState::ReadWrite));
  }
}

TEST_F(PLI_HDF5_File, IsHDF5) {
  {  // create dummy file
    auto h5f = PLI::HDF5::File();
    h5f.create(_filePath);
    h5f.close();
  }

  ASSERT_TRUE(PLI::HDF5::File::isHDF5(_filePath));
}

TEST_F(PLI_HDF5_File, FileExists) {
  {  // create dummy file
    auto h5f = PLI::HDF5::File();
    h5f.create(_filePath);
  }
  EXPECT_TRUE(PLI::HDF5::File::fileExists(_filePath));
}

TEST_F(PLI_HDF5_File, Close) {
  EXPECT_NO_THROW({
    auto h5f = PLI::HDF5::File();
    h5f.create(_filePath);
    h5f.close();
  });
}

TEST_F(PLI_HDF5_File, Reopen) {
  {  // reopen none set file object
    auto h5f = PLI::HDF5::File();
    EXPECT_ANY_THROW(h5f.reopen());
  }

  EXPECT_NO_THROW({  // reopen closed file object
    auto h5f = PLI::HDF5::File();
    h5f.create(_filePath);
    h5f.flush();
    h5f.reopen();
    std::filesystem::remove(_filePath);
  });

  {  // reopen closed file
    auto h5f = PLI::HDF5::File();
    h5f.create(_filePath);
    h5f.close();
    EXPECT_THROW(h5f.reopen(),
                 PLI::HDF5::Exceptions::IdentifierNotValidException);
  }
}

TEST_F(PLI_HDF5_File, Flush) {
  EXPECT_ANY_THROW({  // flush non created file
    auto h5f = PLI::HDF5::File();
    h5f.flush();
    // TODO: specify throw
  });
  std::filesystem::remove(_filePath);

  EXPECT_NO_THROW({  // flush empty file
    auto h5f = PLI::HDF5::File();
    h5f.create(_filePath);
    h5f.flush();
  });
}

TEST_F(PLI_HDF5_File, ID) {
  auto h5f = PLI::HDF5::File();
  h5f.create(_filePath);
  ASSERT_TRUE(h5f.id() >= 0);
}

TEST_F(PLI_HDF5_File, FaplID) {
  auto h5f = PLI::HDF5::File();
  h5f.create(_filePath);
  ASSERT_TRUE(h5f.faplID() >= 0);
}

int main(int argc, char *argv[]) {
  int result = 0;

  MPI_Init(&argc, &argv);
  ::testing::InitGoogleTest(&argc, argv);
  result = RUN_ALL_TESTS();

  MPI_Finalize();
  return result;
}
