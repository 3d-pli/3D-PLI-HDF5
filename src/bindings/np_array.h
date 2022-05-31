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

#pragma once

#include <cassert>
#include <iostream>
#include <vector>

namespace PLI {
namespace HDF5 {
namespace NUMPY {
template <typename T>
class Array {
  /**
   * This Container ist for reading a numpy array. Therefore python will
   * dealocate the memory. Be sure, that the data is passed at runtime of the
   * function, since python can change the memory adress anytime.
   */
 public:
  Array(T *ptr, size_t n, std::vector<size_t> shape)
      : data_{ptr}, size_(n), shape_(shape) {
    assert(shape.size() > 0);
    assert(std::accumulate(shape.begin(), shape.end(), 1ULL,
                           std::multiplies<size_t>()) == n);
  };

  Array() = default;

  // operators
  constexpr const T &operator[](size_t i) const {
    assert(i < size_);
    return data_[i];
  }
  T &operator[](size_t i) {
    assert(i < size_);
    return data_[i];
  }

  // getter
  size_t size() const { return size_; }
  size_t ndim() const { return shape_.size(); }
  const std::vector<size_t> &shape() const { return shape_; }

 private:
  T *const data_;
  size_t size_ = 0;
  std::vector<size_t> shape_{};
};

template <typename T>
Array<T> Array2Container(py::array_t<T, py::array::c_style> &array) {
  auto buffer = array.request();
  assert(buffer.size >= 0);
  return PLI::HDF5::NP::Array<T>(
      reinterpret_cast<T *>(buffer.ptr), buffer.size,
      std::vector<size_t>(buffer.shape.begin(), buffer.shape.end()));
}

template <typename T>
std::vector<T> Array2Vector(py::array_t<T, py::array::c_style> &array) {
  auto buffer = array.request();
  auto size = std::accumulate(buffer.shape.begin(), buffer.shape.end(), 1ULL,
                              std::multiplies<size_t>());
  std::vector<T> data(size);

  for (auto i = 0u; i < data.size(); i++) {
    data[i] = reinterpret_cast<T *>(buffer.ptr)[i];
  }

  return data;
}

template <typename T>
py::array Vec2Array(std::vector<T> *vec,
                    std::vector<size_t> shape = std::vector<size_t>()) {
  if (shape.empty()) shape.push_back(vec->size());

  assert(vec->size() == std::accumulate(shape.begin(), shape.end(), 1ULL,
                                        std::multiplies<size_t>()));

  std::vector<size_t> stride(shape.size(), 0);
  size_t elm_stride = sizeof(T);

  auto shape_it = shape.rbegin();
  auto stride_it = stride.rbegin();
  for (; stride_it != stride.rend(); stride_it++, shape_it++) {
    *stride_it = elm_stride;
    elm_stride *= *shape_it;
  }

  auto capsule = py::capsule(
      vec, [](void *vec) { delete reinterpret_cast<std::vector<T> *>(vec); });

  return py::array_t<T>(shape, stride, vec->data(), capsule);

}  // namespace NP
}  // namespace HDF5
}  // namespace PLI
