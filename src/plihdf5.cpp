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

#include "PLIHDF5/plihdf5.h"

PLI::PLIM::PLIM(PLI::HDF5::File handler, const std::string &dataset) {
  if (PLI::HDF5::Dataset::exists(handler, dataset)) {
    PLI::HDF5::Dataset datasetHandler =
        PLI::HDF5::Dataset::open(handler, dataset);
    m_attrHandler = PLI::HDF5::AttributeHandler(datasetHandler);
  } else {
    throw PLI::HDF5::DatasetNotFoundException(dataset);
  }
}

PLI::PLIM::PLIM(PLI::HDF5::AttributeHandler handler) : m_attrHandler(handler) {}

bool PLI::PLIM::validSolrHDF5(const std::string &solrJSON) { return true; }

void PLI::PLIM::addCreator() {
  std::string username;
#ifdef __GNUC__
  username = std::getenv("USER");
#else
  char username_arr[UNLEN + 1];
  DWORD username_len = UNLEN + 1;
  GetUserName(username_arr, &username_len);
  username = std::string(username_arr);
#endif
  if (m_attrHandler.attributeExists("created_by")) {
    m_attrHandler.deleteAttribute("created_by");
  }
  m_attrHandler.createAttribute("created_by", username);
}

void PLI::PLIM::addID(const std::vector<std::string> &idAttributes) {
  std::string hashCode;

  for (const std::string &attribute : idAttributes) {
    if (m_attrHandler.attributeExists(attribute)) {
      std::vector<std::string> attributeString =
          m_attrHandler.getAttribute<std::string>(attribute);
      std::for_each(attributeString.begin(), attributeString.end(),
                    [&hashCode](const std::string &str) { hashCode += str; });
    } else {
      throw PLI::HDF5::AttributeNotFoundException(attribute);
    }
  }

  // Write the attribute
  if (m_attrHandler.attributeExists("id")) {
    m_attrHandler.deleteAttribute("id");
  }
  m_attrHandler.createAttribute("id", toSHA256(hashCode));
}

void PLI::PLIM::addReference(const PLI::HDF5::AttributeHandler &file) {
  std::vector<std::string> fileID = file.getAttribute<std::string>("id");
  m_attrHandler.createAttribute("reference_images", fileID[0]);
}

void PLI::PLIM::addReference(
    const std::vector<PLI::HDF5::AttributeHandler> &files) {
  std::vector<std::string> fileIDs;
  std::transform(files.begin(), files.end(), std::back_inserter(fileIDs),
                 [](PLI::HDF5::AttributeHandler file) -> std::string {
                   return file.getAttribute<std::string>("id")[0];
                 });
  m_attrHandler.createAttribute("reference_images", fileIDs, {fileIDs.size()});
}

void PLI::PLIM::addSoftware(const std::string &softwareName) {
  if (m_attrHandler.attributeExists("software")) {
    m_attrHandler.deleteAttribute("software");
  }
  m_attrHandler.createAttribute("software", softwareName);
}

void PLI::PLIM::addSoftwareRevision(const std::string &softwareRevision) {
  if (m_attrHandler.attributeExists("software_revision")) {
    m_attrHandler.deleteAttribute("software_revision");
  }
  m_attrHandler.createAttribute("software_revision", softwareRevision);
}

void PLI::PLIM::addSoftwareParameters(const std::string &softwareParameters) {
  if (m_attrHandler.attributeExists("software_parameters")) {
    m_attrHandler.deleteAttribute("software_parameters");
  }
  m_attrHandler.createAttribute("software_parameters", softwareParameters);
}
