# 3D-PLI HDF Library

## Introduction

HDF5 is a widely used file format for storing and sharing scientific data. It is used in the Fiber Architecture Group to store brain data and pyramids of images. The pyramids allow easy visualization of the data with external tools.

However, using HDF5 can be troublesome when using C / C++. The library itself is written in C. While headers for C++ are provided, important features like MPI are not available.

To improve the usability of the library, this repository contains a C++ library wrapping features of HDF5 to make the usage in 3D-PLI projects easier.

## Install instructions

### Requirements

- CMake 3.19 or newer
- HDF5 with C and HL
- MPI
- C++20 compatible compiler

If you want to use linting during the compilation:

- Clang-tidy
- Clang-format
- cppcheck
- cpplint

### Compile the project manually

```bash
git clone https://github.com/3d-pli/3D-PLI-HDF5.git
cd 3D-PLI-HDF5
mkdir build
cd build
cmake ..
make
```

### Install the library from source

```bash
git clone https://github.com/3d-pli/3D-PLI-HDF5.git
cd 3D-PLI-HDF5
mkdir build
cd build
cmake ..
make
make install
```

### Build DEB package

```bash
git clone https://github.com/3d-pli/3D-PLI-HDF5.git
cd 3D-PLI-HDF5
mkdir build
cd build
cmake ..
make
cpack
```

### Install the library as a DEB package

To install the package, either build it yourself or download the packages from the releases / pipeline artifacts.
Then, you can run the following commands to install the package.

```bash
apt-get update -qq
apt-get install -y ./plihdf5_1.0.0_amd64-devel.deb ./plihdf5_1.0.0_amd64-runtime.deb
```

## API documentation

The API documentation is generated from the source code.
The methods itself are documented in the source code. To view the documentation outside of the repository, you can go to the wiki page of the repository: [Here](https://3d-pli.github.io/3D-PLI-HDF5/html/)
