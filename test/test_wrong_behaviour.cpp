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

void removeFile(const std::string &path) {
    int32_t rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0 && std::filesystem::exists(path))
        std::filesystem::remove(path);
    MPI_Barrier(MPI_COMM_WORLD);
}

class PrepareWrongBehaviour : public ::testing::Test {
  protected:
    void SetUp() override {}

    void TearDown() override { removeFile(_filePath); }

    const std::string _filePath =
        std::filesystem::temp_directory_path() / "test_file.h5";
};

TEST_F(PrepareWrongBehaviour, CloseFileBeforeOther) {
    PLI::HDF5::Group group;
    ASSERT_NO_THROW({
        PLI::HDF5::File file = PLI::HDF5::createFile(
            _filePath, PLI::HDF5::File::CreateState::OverrideExisting,
            MPI_COMM_WORLD);

        group = PLI::HDF5::createGroup(file, "/Image");
    });
    ASSERT_THROW(group.create(group, "/NextImage"),
                 PLI::HDF5::Exceptions::IdentifierNotValidException);
    ASSERT_NO_THROW(group.close());
}

int main(int argc, char *argv[]) {
    int result = 0;

    MPI_Init(&argc, &argv);
    ::testing::InitGoogleTest(&argc, argv);
    result = RUN_ALL_TESTS();

    MPI_Finalize();
    return result;
}
