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
- C++17 compatible compiler

If you want to use linting during the compilation:
- Clang-tidy
- Clang-format
- cppcheck
- cpplint

### Compile the project manually

```bash
git clone https://github.com/3d-pli/3D-PLI-HDF5.git
cd 3d-pli-hdf5
mkdir build
cd build
cmake ..
make
```

### Install the library from source  
```bash
git clone https://github.com/3d-pli/3D-PLI-HDF5.git
cd 3d-pli-hdf5
mkdir build
cd build
cmake ..
make
make install
```

### Build DEB package
**TODO**
```bash
git clone https://github.com/3d-pli/3D-PLI-HDF5.git
cd 3d-pli-hdf5
mkdir build
cd build
cmake ..
make
cpack -G DEB
```

### Install the library as a DEB package
**TODO**
```bash
dpkg -i 3d-pli-hdf5_0.1.0-1_amd64.deb
```


## API documentation

The API documentation is generated from the source code.
The methods itself are documented in the source code. To view the documentation outside of the repository, you can go to the wiki page of the repository: [Here](https://jugit.fz-juelich.de/prototypes/3d-pli-hdf5/-/wikis/home)

