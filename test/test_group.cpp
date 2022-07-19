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

class PLI_HDF5_Group : public ::testing::Test {
  protected:
    void SetUp() override {
        int32_t rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        try {
            _file.close();
        } catch (...) {
            // can occur due to testing failures. leave pointer open and
            // continue.
        }
        if (rank == 0 && std::filesystem::exists(_filePath))
            std::filesystem::remove(_filePath);
        MPI_Barrier(MPI_COMM_WORLD);
        _file = PLI::HDF5::createFile(_filePath, MPI_COMM_WORLD);
    }

    void TearDown() override {
        int32_t rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        try {
            _file.close();
        } catch (...) {
            // can occur due to testing failures. leave pointer open and
            // continue.
        }
        if (rank == 0 && std::filesystem::exists(_filePath))
            std::filesystem::remove(_filePath);
        MPI_Barrier(MPI_COMM_WORLD);
    }

    const std::string _filePath =
        std::filesystem::temp_directory_path() / "test_group.h5";
    PLI::HDF5::File _file;
};

TEST_F(PLI_HDF5_Group, Open) {
    { // open existing
        auto grp = PLI::HDF5::createGroup(_file, "foo");
        grp.close();
        EXPECT_NO_THROW(grp = PLI::HDF5::openGroup(_file, "foo"));
    }

    { // open sub path
        auto grp = PLI::HDF5::createGroup(_file, "foo/bar");
        grp.close();
        EXPECT_NO_THROW(grp = PLI::HDF5::openGroup(_file, "foo/bar"));
    }

    { // open sub path
        auto grp = PLI::HDF5::openGroup(_file, "foo");
        EXPECT_NO_THROW(PLI::HDF5::openGroup(grp, "bar"));
    }

    { // open non existing
        EXPECT_THROW(PLI::HDF5::openGroup(_file, "bar"),
                     PLI::HDF5::Exceptions::GroupNotFoundException);
    }
}

TEST_F(PLI_HDF5_Group, Create) {
    { // create
        PLI::HDF5::Group grp;
        EXPECT_NO_THROW(grp = PLI::HDF5::createGroup(_file, "foo"));
    }

    { // create subgrp
        PLI::HDF5::Group grp;
        EXPECT_NO_THROW(grp = PLI::HDF5::createGroup(_file, "foo/bar"));
    }

    { // create subgrp from grp
        auto grp = PLI::HDF5::openGroup(_file, "foo");
        PLI::HDF5::Group subgrp;
        EXPECT_NO_THROW(subgrp = PLI::HDF5::createGroup(grp, "BAR"));
    }

    { // create already existing grp
        PLI::HDF5::Group grp;
        EXPECT_THROW(PLI::HDF5::createGroup(_file, "foo"),
                     PLI::HDF5::Exceptions::GroupExistsException);
    }
}

TEST_F(PLI_HDF5_Group, Exists) {
    { // existing grp
        auto grp = PLI::HDF5::createGroup(_file, "foo");
        grp.close();
        EXPECT_TRUE(PLI::HDF5::Group::exists(_file, "foo"));
    }

    { // none existing grp
        EXPECT_FALSE(PLI::HDF5::Group::exists(_file, "bar"));
    }
}

TEST_F(PLI_HDF5_Group, ID) {}

int main(int argc, char *argv[]) {
    int result = 0;

    MPI_Init(&argc, &argv);
    ::testing::InitGoogleTest(&argc, argv);
    result = RUN_ALL_TESTS();

    MPI_Finalize();
    return result;
}
