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

#include <cmath>
#include <filesystem>

#include "PLIHDF5/dataset.h"
#include "PLIHDF5/file.h"

class PLI_HDF5_Dataset : public ::testing::Test {
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
        _file = PLI::HDF5::createFile(
            _filePath, PLI::HDF5::File::CreateState::OverrideExisting,
            MPI_COMM_WORLD);
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
    }

    const std::vector<size_t> _dims{{128, 128, 4}};
    const std::vector<size_t> _chunk_dims{{128, 128, 4}};
    const std::string _filePath =
        std::filesystem::temp_directory_path() / "test_dataset.h5";
    PLI::HDF5::File _file;
};

TEST_F(PLI_HDF5_Dataset, Constructor) {}
TEST_F(PLI_HDF5_Dataset, Destructor) {}

TEST_F(PLI_HDF5_Dataset, exists) {
    { // checking non existing dset
        EXPECT_FALSE(PLI::HDF5::Dataset::exists(_file, "/Image"));
    }

    { // checking existing dset
        auto dset = _file.createDataset<float>("/Image", _dims, _chunk_dims);
        EXPECT_TRUE(PLI::HDF5::Dataset::exists(_file, "/Image"));
        dset.close();
    }
}

TEST_F(PLI_HDF5_Dataset, close) {
    { // close empty dataset
        auto dset = PLI::HDF5::Dataset();
        EXPECT_NO_THROW(dset.close());
    }

    { // close dataset
        auto dset = _file.createDataset<float>("/Image", _dims, _chunk_dims);
        EXPECT_NO_THROW(dset.close());
    }
}

TEST_F(PLI_HDF5_Dataset, write) {
    { // write closed dset
        auto dset = _file.createDataset<float>("/Image_0", _dims, _chunk_dims);
        dset.close();
        const std::vector<float> data(std::accumulate(
            _dims.begin(), _dims.end(), 1, std::multiplies<int>()));
        const std::vector<size_t> offset{{0, 0, 0}};
        EXPECT_THROW(dset.write(data, offset, _dims),
                     PLI::HDF5::Exceptions::IdentifierNotValidException);
    }

    { // write
        auto dset = _file.createDataset<float>("/Image_1", _dims, _chunk_dims);
        const std::vector<float> data(std::accumulate(
            _dims.begin(), _dims.end(), 1, std::multiplies<std::size_t>()));
        const std::vector<size_t> offset{{0, 0, 0}};
        EXPECT_NO_THROW(dset.write(data, offset, _dims));
        dset.close();
    }

    { // write tile
        auto dset = _file.createDataset<float>("/Image_3", _dims, _chunk_dims);
        const std::vector<size_t> dims{{2, 2, 2}};
        const std::vector<size_t> offset{{1, 1, 1}};
        const std::vector<float> data(std::accumulate(
            dims.begin(), dims.end(), 1, std::multiplies<std::size_t>()));
        EXPECT_NO_THROW(dset.write(data, offset, dims));
        dset.close();
    }

    { // write offset outside of dset
        auto dset = _file.createDataset<float>("/Image_4", _dims, _chunk_dims);
        const std::vector<float> data(std::accumulate(
            _dims.begin(), _dims.end(), 1, std::multiplies<int>()));
        const std::vector<size_t> offset{{1, 1, 1}};
        EXPECT_THROW(dset.write(data, offset, _dims),
                     PLI::HDF5::Exceptions::HDF5RuntimeException);
        dset.close();
    }

    { // Call other write / create methods
        PLI::HDF5::Type type = PLI::HDF5::Type::createType<float>();
        auto dset = _file.createDataset("/Image_5", _dims, _chunk_dims, type);
        const std::vector<float> data(std::accumulate(
            _dims.begin(), _dims.end(), 1, std::multiplies<std::size_t>()));
        const std::vector<size_t> offset{{0, 0, 0}};
        EXPECT_NO_THROW(dset.write(data.data(), offset, _dims, {}, type));
    }

    { // Call other write / create methods
        PLI::HDF5::Type type = PLI::HDF5::Type::createType<float>();
        auto dset = PLI::HDF5::Dataset();
        dset.create(_file, "/Image_6", _dims, _chunk_dims, type);
        const std::vector<float> data(std::accumulate(
            _dims.begin(), _dims.end(), 1, std::multiplies<std::size_t>()));
        const std::vector<size_t> offset{{0, 0, 0}};
        EXPECT_NO_THROW(dset.write<float>(data.data(), offset, _dims));
    }
}

TEST(PLI_HDF5_Dataset_, write_mpi_toggle) {
    int32_t rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    const std::vector<size_t> _dims{{128, 128, 4}};
    const std::vector<size_t> _chunk_dims{{128, 128, 4}};
    const std::string _filePath =
        std::filesystem::temp_directory_path() / "test_dataset.h5";

    if (rank == 0) { // Write with only one rank when using MPI
        try {
            PLI::HDF5::File file = PLI::HDF5::createFile(
                _filePath, PLI::HDF5::File::CreateState::OverrideExisting);
            auto dset =
                file.createDataset<float>("/Image_0", _dims, _chunk_dims);
            const std::vector<float> data(std::accumulate(
                _dims.begin(), _dims.end(), 1, std::multiplies<int>()));
            const std::vector<size_t> offset{{0, 0, 0}};
            EXPECT_NO_THROW(dset.write(data, offset, _dims));

            dset.close();
            file.close();
        } catch (const std::exception &e) {
        }
        if (std::filesystem::exists(_filePath))
            std::filesystem::remove(_filePath);
    }
}

TEST_F(PLI_HDF5_Dataset, type) {
    { // call
        auto dset = _file.createDataset<float>("/Image_1", _dims, _chunk_dims);
        EXPECT_NO_THROW(dset.type());
        dset.close();
    }
}

TEST_F(PLI_HDF5_Dataset, ndims) {
    { // call
        auto dset = _file.createDataset<float>("/Image_1", _dims, _chunk_dims);
        EXPECT_EQ(dset.ndims(), _dims.size());
        dset.close();
    }
}

TEST_F(PLI_HDF5_Dataset, dims) {
    { // call
        auto dset = _file.createDataset<float>("/Image_1", _dims, _chunk_dims);
        EXPECT_EQ(dset.dims(), _dims);
        dset.close();
    }
}

TEST_F(PLI_HDF5_Dataset, id) {
    { // empty dataset
        auto dset = PLI::HDF5::Dataset();
        EXPECT_NO_THROW(dset.id());
        dset.close();
    }
}

TEST_F(PLI_HDF5_Dataset, open) {
    { // create dataset
        auto dset = _file.createDataset<float>("/Image", _dims, _chunk_dims);
        dset.close();

        EXPECT_NO_THROW(dset.open(_file, "/Image"));
        dset.close();
        EXPECT_NO_THROW(dset = _file.openDataset("/Image"));
        dset.close();
    }
}

TEST_F(PLI_HDF5_Dataset, create) {
    { // create dataset
        PLI::HDF5::Dataset dset;
        EXPECT_NO_THROW(
            dset = _file.createDataset<float>("/foo", _dims, _chunk_dims));
        dset.close();
        EXPECT_NO_THROW(dset.create<float>(_file, "/bar", _dims, _chunk_dims));
        dset.close();
    }

    { // recreate dataset
        EXPECT_THROW(_file.createDataset<float>("/foo", _dims, _chunk_dims),
                     PLI::HDF5::Exceptions::DatasetExistsException);
    }

    { // create dataset with grp in path
        EXPECT_THROW(_file.createDataset<float>("/not_existing_grp/dset", _dims,
                                                _chunk_dims),
                     PLI::HDF5::Exceptions::IdentifierNotValidException);
    }

    { // create wrong dims and chunk size
        auto chunk_dims = _dims;
        chunk_dims.pop_back();
        EXPECT_ANY_THROW(
            _file.createDataset<float>("/Image_", _dims, chunk_dims));
    }
}

TEST_F(PLI_HDF5_Dataset, read) {
    // write test dataset
    std::vector<int> data(std::accumulate(_dims.begin(), _dims.end(), 1,
                                          std::multiplies<std::size_t>()));
    std::iota(data.begin(), data.end(), 0);
    const std::vector<size_t> offset{{0, 0, 0}};
    {
        auto dset = _file.createDataset<int>("/Image", _dims, _chunk_dims);
        dset.write(data, offset, _dims);
        dset.close();
    }

    { // read dataset
        auto dset = _file.openDataset("/Image");
        const auto data_in = dset.read<int>(offset, _dims);
        EXPECT_EQ(data_in, data);
        EXPECT_EQ(data_in[data_in.size() - 1], data_in.size() - 1);
        dset.close();
    }

    { // read subset
        auto dset = _file.openDataset("/Image");
        const std::vector<size_t> offset_in{{1, 1, 1}};
        const std::vector<size_t> dims_in{{2, 2, 2}};
        const auto size = std::accumulate(dims_in.begin(), dims_in.end(), 1,
                                          std::multiplies<std::size_t>());
        std::vector<int> data_in;
        std::vector<int> data_comp(size);
        EXPECT_NO_THROW(data_in = dset.read<int>(offset_in, dims_in));
        EXPECT_EQ(data_in.size(), size);

        auto ii = 0u;
        for (auto i = offset_in[0]; i < offset_in[0] + dims_in[0]; i++)
            for (auto j = offset_in[1]; j < offset_in[1] + dims_in[1]; j++)
                for (auto k = offset_in[2]; k < offset_in[2] + dims_in[2];
                     k++) {
                    data_comp[ii++] =
                        data[i * _dims[1] * _dims[2] + j * _dims[2] + k];
                }

        EXPECT_EQ(data_in, data_comp);

        dset.close();
    }
}

TEST_F(PLI_HDF5_Dataset, isChunked) {
    { // create dataset with chunks
        auto dset = _file.createDataset<float>("/Image", _dims, _chunk_dims);
        EXPECT_TRUE(dset.isChunked());
        dset.close();
    }

    { // create dataset without chunks
        auto dset = _file.createDataset<float>("/Image_2", _dims);
        EXPECT_FALSE(dset.isChunked());
        dset.close();
    }
}

TEST_F(PLI_HDF5_Dataset, viewDimConversion) {
    // dims to view
    {
        std::vector<size_t> offset = {{}};
        std::vector<size_t> dim = {{}};
        PLI::HDF5::Dataset::View view;
        EXPECT_NO_THROW(view = PLI::HDF5::Dataset::toView(offset, dim));
        EXPECT_EQ(view.size(), offset.size());
    }

    {
        std::vector<size_t> offset = {0};
        std::vector<size_t> dim = {1};
        PLI::HDF5::Dataset::View view;
        EXPECT_NO_THROW(view = PLI::HDF5::Dataset::toView(offset, dim));
        EXPECT_EQ(view.size(), offset.size());
        EXPECT_EQ(view[0].start, offset[0]);
        EXPECT_EQ(view[0].stop, offset[0] + dim[0]);
        EXPECT_EQ(view[0].step, 1);
    }

    {
        std::vector<size_t> offset = {{0, 1}};
        std::vector<size_t> dim = {{10, 12}};
        PLI::HDF5::Dataset::View view;
        EXPECT_NO_THROW(view = PLI::HDF5::Dataset::toView(offset, dim));
        EXPECT_EQ(view.size(), offset.size());
        for (size_t i = 0; i < view.size(); i++) {
            EXPECT_EQ(view[i].start, offset[i]);
            EXPECT_EQ(view[i].stop, offset[i] + dim[i]);
            EXPECT_EQ(view[i].step, 1);
        }
    }

    {
        std::vector<size_t> offset = {{0, 1}};
        std::vector<size_t> dim = {{10, 12}};
        std::vector<size_t> stride = {{1, 2}};
        PLI::HDF5::Dataset::View view;
        EXPECT_NO_THROW(view = PLI::HDF5::Dataset::toView(offset, dim, stride));
        EXPECT_EQ(view.size(), offset.size());
        for (size_t i = 0; i < view.size(); i++) {
            EXPECT_EQ(view[i].start, offset[i]);
            EXPECT_EQ(view[i].stop, offset[i] + dim[i] * stride[i]);
            EXPECT_EQ(view[i].step, stride[i]);
        }
    }

    // view to dims
    {
        std::vector<size_t> offset;
        std::vector<size_t> dim;
        std::vector<size_t> stride;
        PLI::HDF5::Dataset::View view;
        EXPECT_NO_THROW(std::tie(offset, dim, stride) =
                            PLI::HDF5::Dataset::toOffsetAndDim(view));
        EXPECT_EQ(view.size(), offset.size());
        EXPECT_EQ(view.size(), dim.size());
        EXPECT_EQ(view.size(), stride.size());
    }

    {
        std::vector<size_t> offset;
        std::vector<size_t> dim;
        std::vector<size_t> stride;
        PLI::HDF5::Dataset::View view{
            {std::vector<PLI::HDF5::Dataset::Slice>{{0, 1, 1}}}};
        EXPECT_NO_THROW(std::tie(offset, dim, stride) =
                            PLI::HDF5::Dataset::toOffsetAndDim(view));
        EXPECT_EQ(view.size(), offset.size());
        EXPECT_EQ(view.size(), dim.size());
        EXPECT_EQ(view.size(), stride.size());
    }

    {
        std::vector<size_t> offset = {{0, 1}};
        std::vector<size_t> dim = {{10, 12}};
        std::vector<size_t> stride = {{1, 2}};
        PLI::HDF5::Dataset::View view;
        view = PLI::HDF5::Dataset::toView(offset, dim, stride);

        auto [offset_, dim_, stride_] =
            PLI::HDF5::Dataset::toOffsetAndDim(view);

        std::cerr << dim[0] << ", " << dim[1] << std::endl;
        std::cerr << dim_[0] << ", " << dim_[1] << std::endl;
        EXPECT_EQ(offset, offset_);
        EXPECT_EQ(dim, dim_);
        EXPECT_EQ(stride, stride_);
    }
}

TEST_F(PLI_HDF5_Dataset, viewsFromDimensions) {
    {
        std::vector<size_t> chunkDims = {{2048, 2048, 9}};
        std::vector<size_t> dataDims = {
            {chunkDims[0] * 2, chunkDims[1] * 2, 9}};

        auto chunks =
            PLI::HDF5::Dataset::viewsFromDimensions(dataDims, chunkDims);
        EXPECT_EQ(chunks.size(), 4);
        EXPECT_EQ(chunks[0][0], PLI::HDF5::Dataset::Slice(0, chunkDims[0], 1));
        EXPECT_EQ(chunks[0][1], PLI::HDF5::Dataset::Slice(0, chunkDims[1], 1));
        EXPECT_EQ(chunks[0][2], PLI::HDF5::Dataset::Slice(0, chunkDims[2], 1));

        EXPECT_EQ(chunks[1][0], PLI::HDF5::Dataset::Slice(0, chunkDims[0], 1));
        EXPECT_EQ(chunks[1][1],
                  PLI::HDF5::Dataset::Slice(chunkDims[1], 2 * chunkDims[1], 1));
        EXPECT_EQ(chunks[1][2], PLI::HDF5::Dataset::Slice(0, chunkDims[2], 1));

        EXPECT_EQ(chunks[2][0],
                  PLI::HDF5::Dataset::Slice(chunkDims[0], 2 * chunkDims[0], 1));
        EXPECT_EQ(chunks[2][1], PLI::HDF5::Dataset::Slice(0, chunkDims[1], 1));
        EXPECT_EQ(chunks[2][2], PLI::HDF5::Dataset::Slice(0, chunkDims[2], 1));

        EXPECT_EQ(chunks[3][0],
                  PLI::HDF5::Dataset::Slice(chunkDims[0], 2 * chunkDims[0], 1));
        EXPECT_EQ(chunks[3][1],
                  PLI::HDF5::Dataset::Slice(chunkDims[1], 2 * chunkDims[1], 1));
        EXPECT_EQ(chunks[3][2], PLI::HDF5::Dataset::Slice(0, chunkDims[2], 1));
    }

    // TODO(felix): implement for new slice design
    // {
    //     std::vector<size_t> chunkDims = {{2048, 2048, 9}};
    //     std::vector<size_t> dataDims = {
    //         {chunkDims[0] * 2, chunkDims[1] * 1, 9}};

    //     auto chunks =
    //         PLI::HDF5::Dataset::viewsFromDimensions(dataDims, chunkDims);
    //     EXPECT_EQ(chunks.size(), 2);
    //     EXPECT_TRUE(chunks[0].offset == std::vector<size_t>({{0, 0, 0}}));
    //     for (auto chunk : chunks) {
    //         EXPECT_TRUE(chunk.dim == chunkDims);
    //     }
    // }
    // {
    //     std::vector<size_t> chunkDims = {{2048, 2048, 9}};
    //     std::vector<size_t> dataDims = {
    //         {chunkDims[0] * 1, chunkDims[1] * 2, 9}};

    //     auto chunks =
    //         PLI::HDF5::Dataset::viewsFromDimensions(dataDims, chunkDims);
    //     EXPECT_EQ(chunks.size(), 2);
    //     EXPECT_TRUE(chunks[0].offset == std::vector<size_t>({{0, 0, 0}}));
    //     for (auto chunk : chunks) {
    //         EXPECT_TRUE(chunk.dim == chunkDims);
    //     }
    // }
    // {
    //     std::vector<size_t> chunkDims = {{4096, 4096, 1}};
    //     std::vector<size_t> dataDims = {{6144, 8192, 9}};

    //     auto chunks =
    //         PLI::HDF5::Dataset::viewsFromDimensions(dataDims, chunkDims);
    //     EXPECT_EQ(chunks.size(),
    //               std::ceil(dataDims[0] / static_cast<double>(chunkDims[0]))
    //               *
    //                   dataDims[1] / static_cast<double>(chunkDims[1]) *
    //                   dataDims[2] / static_cast<double>(chunkDims[2]));
    //     EXPECT_TRUE(chunks[0].dim == chunkDims);
    //     EXPECT_TRUE(chunks[0].offset == std::vector<size_t>({{0, 0, 0}}));

    //     for (size_t i = 0; i < dataDims[2]; i++) {
    //         EXPECT_TRUE(chunks[0 * dataDims[2] + i].offset ==
    //                     std::vector<size_t>({{0, 0, i}}));
    //         EXPECT_TRUE(chunks[1 * dataDims[2] + i].offset ==
    //                     std::vector<size_t>({{0, chunkDims[1], i}}));
    //         EXPECT_TRUE(chunks[2 * dataDims[2] + i].offset ==
    //                     std::vector<size_t>({{chunkDims[0], 0, i}}));
    //         EXPECT_TRUE(chunks[3 * dataDims[2] + i].offset ==
    //                     std::vector<size_t>({{chunkDims[0], chunkDims[1],
    //                     i}}));

    //         EXPECT_TRUE(chunks[0 * dataDims[2] + i].dim == chunkDims);
    //         EXPECT_TRUE(chunks[1 * dataDims[2] + i].dim == chunkDims);
    //         EXPECT_TRUE(chunks[2 * dataDims[2] + i].dim ==
    //                     std::vector<size_t>(
    //                         {{dataDims[0] - chunkDims[0], chunkDims[1],
    //                         1}}));
    //         EXPECT_TRUE(chunks[3 * dataDims[2] + i].dim ==
    //                     std::vector<size_t>(
    //                         {{dataDims[0] - chunkDims[0], chunkDims[1],
    //                         1}}));
    //     }
    // }
}

// TEST_F(PLI_HDF5_Dataset, chunk_offset) {
//     {
//         std::vector<size_t> chunkDims = {{2048, 2048, 9}};
//         std::vector<size_t> dataDims = chunkDims;
//         std::vector<size_t> offset = {{0, 0, 0}};

//         auto chunks = PLI::HDF5::Dataset::viewsFromDimensions(
//             dataDims, chunkDims, offset);
//         EXPECT_EQ(chunks.size(), 1);
//         EXPECT_TRUE(chunks[0].offset == offset);
//         EXPECT_TRUE(chunks[0].dim == chunkDims);
//     }
//     {
//         std::vector<size_t> chunkDims = {{2048, 2048, 9}};
//         std::vector<size_t> dataDims = chunkDims;
//         std::vector<size_t> offset = {{1, 1, 1}};

//         auto chunks = PLI::HDF5::Dataset::viewsFromDimensions(
//             dataDims, chunkDims, offset);
//         EXPECT_EQ(chunks.size(), 1);
//         EXPECT_TRUE(chunks[0].offset == offset);

//         std::vector<size_t> diff(dataDims.size());
//         std::transform(dataDims.cbegin(), dataDims.cend(), offset.cbegin(),
//                        diff.begin(), std::minus<>{});
//         EXPECT_TRUE(chunks[0].dim == diff);
//     }
// }

// TEST_F(PLI_HDF5_Dataset, size_diff) {
//     {
//         std::vector<size_t> chunkDims = {{2048, 2048, 9}};
//         std::vector<size_t> dataDims = chunkDims;
//         dataDims[0] += 1;

//         auto chunks =
//             PLI::HDF5::Dataset::viewsFromDimensions(dataDims, chunkDims);
//         EXPECT_EQ(chunks.size(), 2);
//         EXPECT_TRUE(chunks[0].offset == std::vector<size_t>({{0, 0, 0}}));
//         EXPECT_TRUE(chunks[0].dim == chunkDims);
//         EXPECT_TRUE(chunks[1].offset == std::vector<size_t>({{2048, 0, 0}}));
//         EXPECT_TRUE(chunks[1].dim == std::vector<size_t>({{1, 2048, 9}}));
//     }
// }

TEST_F(PLI_HDF5_Dataset, chunkDims) {
    { // create dataset with chunks
        auto dset = _file.createDataset<float>("/Image", _dims, _chunk_dims);
        EXPECT_EQ(dset.chunkDims(), _chunk_dims);
        dset.close();
    }

    { // create dataset without chunks
        auto dset = _file.createDataset<float>("/Image_2", _dims);
        EXPECT_THROW(dset.chunkDims(),
                     PLI::HDF5::Exceptions::HDF5RuntimeException);
        dset.close();
    }
}

int main(int argc, char *argv[]) {
    int result = 0;

    MPI_Init(&argc, &argv);
    ::testing::InitGoogleTest(&argc, argv);
    result = RUN_ALL_TESTS();

    MPI_Finalize();
    return result;
}
