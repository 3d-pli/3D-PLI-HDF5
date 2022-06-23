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

#include <hdf5.h>

#include <exception>
#include <string>

/**
 * @brief The PLI namespace
 */
namespace PLI {
/**
 * @brief The HDF5 namespace
 */
namespace HDF5 {
/**
 * @brief The exception namespace for errors of the API
 */
namespace Exceptions {
class DatasetNotFoundException : public std::exception {
  public:
    explicit DatasetNotFoundException(const std::string &message)
        : std::exception(), m_message(message) {}
    virtual ~DatasetNotFoundException() noexcept {}
    virtual const char *what() const noexcept { return m_message.c_str(); }

  private:
    std::string m_message;
};

class IdentifierNotValidException : public std::exception {
  public:
    explicit IdentifierNotValidException(const std::string &message)
        : std::exception(), m_message(message) {}
    virtual ~IdentifierNotValidException() noexcept {}
    virtual const char *what() const noexcept { return m_message.c_str(); }

  private:
    std::string m_message;
};

class AttributeNotFoundException : public std::exception {
  public:
    explicit AttributeNotFoundException(const std::string &message)
        : std::exception(), m_message(message) {}
    virtual ~AttributeNotFoundException() noexcept {}
    virtual const char *what() const noexcept { return m_message.c_str(); }

  private:
    std::string m_message;
};

class AttributeExistsException : public std::exception {
  public:
    explicit AttributeExistsException(const std::string &message)
        : std::exception(), m_message(message) {}
    virtual ~AttributeExistsException() noexcept {}
    virtual const char *what() const noexcept { return m_message.c_str(); }

  private:
    std::string m_message;
};

class DatasetExistsException : public std::exception {
  public:
    explicit DatasetExistsException(const std::string &message)
        : std::exception(), m_message(message) {}
    virtual ~DatasetExistsException() noexcept {}
    virtual const char *what() const noexcept { return m_message.c_str(); }

  private:
    std::string m_message;
};

class FileNotFoundException : public std::exception {
  public:
    explicit FileNotFoundException(const std::string &message)
        : std::exception(), m_message(message) {}
    virtual ~FileNotFoundException() noexcept {}
    virtual const char *what() const noexcept { return m_message.c_str(); }

  private:
    std::string m_message;
};

class FileExistsException : public std::exception {
  public:
    explicit FileExistsException(const std::string &message)
        : std::exception(), m_message(message) {}
    virtual ~FileExistsException() noexcept {}
    virtual const char *what() const noexcept { return m_message.c_str(); }

  private:
    std::string m_message;
};

class GroupExistsException : public std::exception {
  public:
    explicit GroupExistsException(const std::string &message)
        : std::exception(), m_message(message) {}
    virtual ~GroupExistsException() noexcept {}
    virtual const char *what() const noexcept { return m_message.c_str(); }

  private:
    std::string m_message;
};

class GroupNotFoundException : public std::exception {
  public:
    explicit GroupNotFoundException(const std::string &message)
        : std::exception(), m_message(message) {}
    virtual ~GroupNotFoundException() noexcept {}
    virtual const char *what() const noexcept { return m_message.c_str(); }

  private:
    std::string m_message;
};

class DimensionMismatchException : public std::exception {
  public:
    explicit DimensionMismatchException(const std::string &message)
        : std::exception(), m_message(message) {}
    virtual ~DimensionMismatchException() noexcept {}
    virtual const char *what() const noexcept { return m_message.c_str(); }

  private:
    std::string m_message;
};

class InvalidHDF5FileException : public std::exception {
  public:
    explicit InvalidHDF5FileException(const std::string &message)
        : std::exception(), m_message(message) {}
    virtual ~InvalidHDF5FileException() noexcept {}
    virtual const char *what() const noexcept { return m_message.c_str(); }

  private:
    std::string m_message;
};

class HDF5RuntimeException : public std::exception {
  public:
    explicit HDF5RuntimeException(const std::string &message)
        : std::exception(), m_message(message) {}
    virtual ~HDF5RuntimeException() noexcept {}
    virtual const char *what() const noexcept { return m_message.c_str(); }

  private:
    std::string m_message;
};
} // namespace Exceptions
void checkHDF5Call(const herr_t hdf5ReturnValue,
                   const std::string &message = "None");
void checkHDF5Ptr(const hid_t hdf5Ptr, const std::string &message = "None");
} // namespace HDF5
} // namespace PLI
