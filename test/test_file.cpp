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
#include "testEnvironment.h"

TEST(PLI_HDF5_File, Constructor) {
  EXPECT_NO_THROW({ auto h5f = PLI::HDF5::File(); });

  EXPECT_NO_THROW({
    auto h5f_0 = PLI::HDF5::File();
    auto h5f_1 = PLI::HDF5::File(h5f_0);
  });

  // TODO: File(const hid_t filePtr, const hid_t faplID);
}

TEST(PLI_HDF5_File, Destructor) {
  EXPECT_NO_THROW({  // invoke deconstroctor
    auto h5f = PLI::HDF5::File();
  });
}

TEST(PLI_HDF5_File, Create) {
  {  // create file
    auto h5f = PLI::HDF5::File();
    h5f.create(kTmpFile);
  }

  {  // create existing file
    auto h5f = PLI::HDF5::File();
    EXPECT_THROW(h5f.create(kTmpFile),
                 PLI::HDF5::Exceptions::FileExistsException);
  }
}

TEST(PLI_HDF5_File, Open) {
  {  // create dummy file
    auto h5f = PLI::HDF5::File();
    h5f.create(kTmpFile);
  }

  {  // open dummy file
    auto h5f = PLI::HDF5::File();
    EXPECT_TRUE(h5f.open(kTmpFile, PLI::HDF5::File::OpenState::ReadWrite));
  }
}

TEST(PLI_HDF5_File, IsHDF5) {
  {  // create dummy file
    auto h5f = PLI::HDF5::File();
    h5f.create(kTmpFile);
  }

  ASSERT_TRUE(PLI::HDF5::File::isHDF5(kTmpFile));
}

TEST(PLI_HDF5_File, FileExists) {
  {  // create dummy file
    auto h5f = PLI::HDF5::File();
    h5f.create(kTmpFile);
  }
  EXPECT_TRUE(PLI::HDF5::File::fileExists(kTmpFile));
}

TEST(PLI_HDF5_File, Close) {
  EXPECT_NO_THROW({
    auto h5f = PLI::HDF5::File();
    h5f.create(kTmpFile);
    h5f.close();
  });
}

TEST(PLI_HDF5_File, Reopen) {
  {  // reopen non set file object
    auto h5f = PLI::HDF5::File();
    EXPECT_ANY_THROW(h5f.reopen());
  }

  {  // reopen closes file object
    auto h5f = PLI::HDF5::File();
    h5f.create(kTmpFile);
    h5f.close();
    h5f.reopen();
    std::filesystem::remove(kTmpFile);
  }

  {  // reopen removed file
    auto h5f = PLI::HDF5::File();
    h5f.create(kTmpFile);
    h5f.close();
    std::filesystem::remove(kTmpFile);
    EXPECT_ANY_THROW(
        h5f.reopen());  // ???: InvalidHDF5FileException or HDF5RuntimeException
  }
}

TEST(PLI_HDF5_File, Flush) {
  EXPECT_ANY_THROW({  // flush non created file
    auto h5f = PLI::HDF5::File();
    h5f.flush();
    // TODO: specify throw
  });
  std::filesystem::remove(kTmpFile);

  EXPECT_NO_THROW({  // flush empty file
    auto h5f = PLI::HDF5::File();
    h5f.create(kTmpFile);
    h5f.flush();
  });
}

TEST(PLI_HDF5_File, ID) {
  auto h5f = PLI::HDF5::File();
  h5f.create(kTmpFile);
  ASSERT_TRUE(h5f.id() >= 0);
}

TEST(PLI_HDF5_File, FaplID) {
  auto h5f = PLI::HDF5::File();
  h5f.create(kTmpFile);
  ASSERT_TRUE(h5f.faplID() >= 0);
}

int main(int argc, char *argv[]) {
  int result = 0;

  ::testing::InitGoogleTest(&argc, argv);
  ::testing::AddGlobalTestEnvironment(new MPIEnvironment);

  result = RUN_ALL_TESTS();
  return result;
}
