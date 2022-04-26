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

TEST(TestFile, Create)
{
  auto h5f = PLI::HDF5::File();
  h5f.create(kTmpFile);
}

TEST(TestFile, Open)
{
  {
    auto h5f = PLI::HDF5::File();
    h5f.create(kTmpFile);
  }

  auto h5f = PLI::HDF5::File();
  h5f.open(kTmpFile, 1);
}

TEST(TestFile, IsHDF5)
{
  {
    auto h5f = PLI::HDF5::File();
    h5f.create(kTmpFile);
  }
  ASSERT_TRUE(PLI::HDF5::File::isHDF5(kTmpFile));
}

TEST(TestFile, FileExists)
{
  auto h5f = PLI::HDF5::File();
  h5f.create(kTmpFile);
  EXPECT_THROW(h5f.create(kTmpFile), PLI::HDF5::FileExistsException);
}

TEST(TestFile, Close)
{
  auto h5f = PLI::HDF5::File();
  h5f.create(kTmpFile);
  h5f.close();
}

TEST(TestFile, Reopen)
{

  {
    auto h5f = PLI::HDF5::File();
    EXPECT_ANY_THROW(h5f.reopen());
  }

  {
    auto h5f = PLI::HDF5::File();
    h5f.create(kTmpFile);
    h5f.close();
    h5f.reopen();
    std::filesystem::remove(kTmpFile);
  }

  {
    auto h5f = PLI::HDF5::File();
    h5f.create(kTmpFile);
    h5f.close();
    std::filesystem::remove(kTmpFile);
    EXPECT_THROW(h5f.reopen()); // ???: InvalidHDF5FileException or HDF5RuntimeException
  }
}

TEST(TestFile, Flush)
{
  auto h5f = PLI::HDF5::File();
  h5f.create(kTmpFile);
  h5f.flush();
}

TEST(TestFile, ID)
{
  auto h5f = PLI::HDF5::File();
  h5f.create(kTmpFile);
  ASSERT_TRUE(h5f.id() != 0);
}

int main(int argc, char *argv[])
{
  int result = 0;

  ::testing::InitGoogleTest(&argc, argv);
  ::testing::AddGlobalTestEnvironment(new MPIEnvironment);

  result = RUN_ALL_TESTS();
  return result;
}
