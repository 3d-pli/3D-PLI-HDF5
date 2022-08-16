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

#include <string>
#include <vector>

#include "PLIHDF5/attributes.h"
#include "PLIHDF5/file.h"

/**
 * @brief The PLI namespace
 */
namespace PLI {

namespace HDF5 {
/**
 * @brief The PLIM class contains a few functions to make the integration of a
 * generated HDF5 file easier with the Solr database used in the work group.
 * @details The class is a wrapper around the HDF5 library. It provides
 * functions to add the necessary information to the HDF5 file to make it
 * compatible with the Solr database.
 */
class PLIM {
  public:
    /**
     * @brief Constructor
     * @param file HDF5 file object used to store the information.
     * @param dataset HDF5 dataset object used to store the information.
     */
    explicit PLIM(PLI::HDF5::File file, const std::string &dataset);
    /**
     * @brief Constructor
     * @param dataset AttributeHandler containing the attributes of the desired
     * dataset where information is stored.
     */
    explicit PLIM(PLI::HDF5::AttributeHandler dataset);
    /**
     * @brief Adds the creator of the HDF5 file to the HDF5 file.
     */
    void addCreator();
    /**
     * @brief Adds the ID of the HDF5 file to the HDF5 file.
     * @param idAttributes List of attributes that are used to generate the ID.
     */
    void addID(const std::vector<std::string> &idAttributes = {});
    /**
     * @brief Adds the reference files used to generate this HDF5 file.
     * @param file Reference attribute handler used to generate this HDF5 file.
     */
    void addReference(const PLI::HDF5::AttributeHandler &file);
    /**
     * @brief Adds the reference files used to generate this HDF5 file.
     * @param files List of reference attribute handlers used to generate this
     * HDF5 file.
     */
    void addReference(const std::vector<PLI::HDF5::AttributeHandler> &files);
    /**
     * @brief Adds the software name.
     * @param softwareName Software name used to generate this HDF5 file.
     */
    void addSoftware(const std::string &softwareName);
    /**
     * @brief Adds the software revision.
     * @param softwareRevision Software revision used to generate this HDF5
     * file.
     */
    void addSoftwareRevision(const std::string &softwareRevision);
    /**
     * @brief Adds the software parameters.
     * @param softwareParameters Software parameters used to generate this HDF5
     * file.
     */
    void addSoftwareParameters(const std::string &softwareParameters);
    /**
     * @brief Adds the creation time of the HDF5 file
     */
    void addCreationTime();
    /**
     * @brief Adds the name of the modality stored in the HDF5 file.
     * @param modality Modality name stored.
     */
    void addImageModality(const std::string &modalityName);

  private:
    PLI::HDF5::AttributeHandler m_attrHandler;
};
} // namespace HDF5
} // namespace PLI
