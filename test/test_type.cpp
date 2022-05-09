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

#include <string>

#include "PLIHDF5/type.h"

template <typename T>
class TypeTest : public ::testing::Test {
 protected:
  TypeTest() : m_type(PLI::HDF5::Type::createType<T>()) {}
  PLI::HDF5::Type m_type;
};
typedef ::testing::Types<uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t,
                         int32_t, int64_t, float, double, std::string>
    TestTypes;

TYPED_TEST_SUITE(TypeTest, TestTypes);

TYPED_TEST(TypeTest, CreateType) {
  EXPECT_TRUE(PLI::HDF5::Type::createType<TypeParam>() == this->m_type);
}

TYPED_TEST(TypeTest, CreateTypeFromName) {}

TYPED_TEST(TypeTest, CreateTypeFromID) {}

int main(int argc, char* argv[]) {
  int result = 0;

  MPI_Init(&argc, &argv);
  ::testing::InitGoogleTest(&argc, argv);
  result = RUN_ALL_TESTS();

  MPI_Finalize();
  return result;
}
