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

#include <algorithm>
#include <string>
#include <vector>

#include "PLIHDF5/attributes.h"
#include "PLIHDF5/dataset.h"
#include "PLIHDF5/exceptions.h"
#include "PLIHDF5/file.h"
#include "PLIHDF5/group.h"
#include "PLIHDF5/sha256.h"

namespace PLI {
class PLIM {
 public:
  explicit PLIM(PLI::HDF5::File file, const std::string& dataset);
  explicit PLIM(PLI::HDF5::AttributeHandler dataset);

  bool validSolrHDF5(const std::string& solrJSON);
  void addCreator();
  void addID(const std::vector<std::string>& idAttributes);
  void addReference(const PLI::HDF5::AttributeHandler& file);
  void addReference(const std::vector<PLI::HDF5::AttributeHandler>& files);
  void addSoftware(const std::string& softwareName);
  void addSoftwareRevision(const std::string& softwareRevision);
  void addSoftwareParameters(const std::string& softwareParameters);

 private:
  PLI::HDF5::AttributeHandler m_attrHandler;
};
}  // namespace PLI
