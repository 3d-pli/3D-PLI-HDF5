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
#include <numeric>

#include "PLIHDF5/dataset.h"
#include "PLIHDF5/file.h"

class PLI_HDF5_ODF : public ::testing::Test {
 protected:
  void SetUp() override {
    int32_t rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0 && std::filesystem::exists(_filePath))
      std::filesystem::remove(_filePath);
    MPI_Barrier(MPI_COMM_WORLD);
  }

  void TearDown() override {
    int32_t rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0 && std::filesystem::exists(_filePath))
      std::filesystem::remove(_filePath);
    MPI_Barrier(MPI_COMM_WORLD);
  }

  const std::string _filePath =
      std::filesystem::temp_directory_path() / "test_odf.h5";
};

TEST_F(PLI_HDF5_ODF, odf) {
  const int n_coeff = 28;

  {  // write
    auto file = PLI::HDF5::createFile(_filePath);

    // generate temporary data
    const std::vector<hsize_t> chunk_dims{{5, 5, 5, n_coeff}};
    const std::vector<hsize_t> dims{{10, 10, 10, n_coeff}};
    const std::vector<hsize_t> offset{{0, 0, 0, 0}};
    std::vector<float> data(std::accumulate(dims.begin(), dims.end(), 1,
                                            std::multiplies<size_t>()));
    std::fill(data.begin(), data.end(), 0);

    for (hsize_t i = 3; i < 7; i++)
      for (hsize_t j = 3; j < 7; j++)
        for (hsize_t k = 3; k < 7; k++)
          data[i * dims[1] * dims[2] * dims[3] + j * dims[2] * dims[3] +
               k * dims[3]] = 1;

    auto dset = PLI::HDF5::createDataset<float>(file, "/ODF", dims, chunk_dims);
    EXPECT_NO_THROW(dset.write(data, offset, dims));

    dset.close();
    file.close();
  }

  {  // read
    auto file = PLI::HDF5::openFile(_filePath);
    auto dset = PLI::HDF5::openDataset(file, "/ODF");

    // read sub dataset
    const std::vector<hsize_t> offset{{0, 0, 0, 0}};
    const std::vector<hsize_t> count{{1, 5, 5, n_coeff}};
    auto data = dset.read<float>(offset, count);

    // read full dataset
    auto dim = dset.dims();
    auto data_all = dset.readFullDataset<float>();

    // accessing elements:
    // data_all[z*dim[1]*dim[2]*dim[3]+y*dim[2]*dim[3]+x*dim[3]+c]

    dset.close();
    file.close();
  }
}

int main(int argc, char* argv[]) {
  int result = 0;

  MPI_Init(&argc, &argv);
  ::testing::InitGoogleTest(&argc, argv);
  result = RUN_ALL_TESTS();

  MPI_Finalize();
  return result;
}
