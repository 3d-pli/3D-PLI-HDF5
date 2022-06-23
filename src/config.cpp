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

#include "PLIHDF5/config.h"

#include <iostream>

std::unique_ptr<PLI::HDF5::Config> PLI::HDF5::Config::instance = nullptr;

PLI::HDF5::Config::Config() {
    // Check if shell variable PLIHDF5_ATTRIBUTE_SETTINGS_FILE_PATH is set.
    // If yes, use it as config file path.
    // If not, use default config file path.
    const char *env = std::getenv("PLIHDF5_ATTRIBUTE_SETTINGS_FILE_PATH");
    if (env != nullptr) {
        configFilePath = env;
    } else {
        configFilePath = "";
    }

    _exceptionPrintingEnabled = true;
    checkHDF5Call(H5Eget_auto2(H5E_DEFAULT, &_exceptionFunction, &_clientData));
}

PLI::HDF5::Config *PLI::HDF5::Config::getInstance() {
    if (instance == nullptr) {
        instance = std::unique_ptr<Config>(new Config());
    }
    return instance.get();
}

std::string PLI::HDF5::Config::getConfigFilePath() noexcept {
    if (configFilePath.empty()) {
        for (auto &folder : installedConfigFolderOptions) {
            std::string path = std::string(folder) + "/" +
                               std::string(installedConfigFilePath);
            if (std::filesystem::exists(path)) {
                configFilePath = path;
                break;
            }
        }
    }
    return configFilePath;
}

void PLI::HDF5::Config::setConfigFilePath(const std::string &path) noexcept {
    std::cout << path << std::endl;
    configFilePath = path;
}

std::vector<std::string> PLI::HDF5::Config::getExcludedCopyAttributes() {
    using json = nlohmann::json;
    json j;

    if (!std::filesystem::exists(getConfigFilePath())) {
        return {};
    }
    std::ifstream configFile(getConfigFilePath());
    if (!configFile.is_open()) {
        throw std::runtime_error("Config file " + getConfigFilePath() +
                                 " could not be opened.");
    }
    configFile >> j;

    std::vector<std::string> excludedCopyAttributes;
    std::copy(j["excluded_copy_attributes"].begin(),
              j["excluded_copy_attributes"].end(),
              std::back_inserter(excludedCopyAttributes));
    return excludedCopyAttributes;
}

std::vector<std::string> PLI::HDF5::Config::getIDAttributes() {
    using json = nlohmann::json;
    json j;

    if (!std::filesystem::exists(getConfigFilePath())) {
        throw std::runtime_error("Config file " + getConfigFilePath() +
                                 " does not exist.");
    }
    std::ifstream configFile(getConfigFilePath());
    if (!configFile.is_open()) {
        throw std::runtime_error("Config file " + getConfigFilePath() +
                                 " could not be opened.");
    }
    configFile >> j;

    std::vector<std::string> idAttributes;
    std::copy(j["id_attributes"].begin(), j["id_attributes"].end(),
              std::back_inserter(idAttributes));
    return idAttributes;
}

bool PLI::HDF5::Config::exceptionPrintingEnabled() const {
    return _exceptionPrintingEnabled;
}

void PLI::HDF5::Config::setExceptionPrintingEnabled(const bool enable) {
    _exceptionPrintingEnabled = enable;
    if (enable) {
        checkHDF5Call(
            H5Eset_auto2(H5E_DEFAULT, _exceptionFunction, _clientData));
    } else {
        checkHDF5Call(H5Eset_auto2(H5E_DEFAULT, nullptr, nullptr));
    }
}
