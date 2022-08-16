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

#include "PLIHDF5/sha512.h"

TEST(TestSHA512, ToSHA512) {
    std::string string = "Hello World!";
    std::string expected =
        "861844d6704e8573fec34d967e20bcfef3d424cf48be04e6dc08f2bd58c72974337101"
        "5ead891cc3cf1c9d34b49264b510751b1ff9e537937bc46b5d6ff4ecc8";
    std::string actual = PLI::toSHA512(string);
    EXPECT_EQ(expected, actual);
}

int main(int argc, char *argv[]) {
    int result = 0;

    MPI_Init(&argc, &argv);
    ::testing::InitGoogleTest(&argc, argv);
    result = RUN_ALL_TESTS();

    MPI_Finalize();
    return result;
}
