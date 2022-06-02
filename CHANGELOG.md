# v1.0.1
## Added
    - Added methods in PLI::HDF5::Dataset
        - Added PLI::HDF5::Dataset::create without template but with PLI::HDF5::Type
        - Added PLI::HDF5::Dataset::write with void* instead of std::vector<T>
        - Added PLI::HDF5::Dataset::write without template but with PLI::HDF5::Type and void* as the data pointer
    - Added PLI::HDF5::createDataset without template but with PLI::HDF5::Type
    - Added PLI::PLIM::addCreationTime and PLI::PLIM::addImageModality
    - Added tests for non MPI execution
    - Added tests for the new methods to create datasets

## Changed
    - If MPI is not initialized during the creation / opening of a file / dataset, the access token will not use MPI. This allows the usage of the toolbox in programs not using MPI.
    - Refactoring of file and dataset methods using the HDF5 MPI implementation to keep the DRY principle.
    - CopyAllTo and CopyAllFrom will now throw an exception after it tried to copy all attributes.
    - One element attributes are now saved as H5S_SCALAR instead of a vector    

## Fixed
    - Fixed issues which could result in SegFaults in rare cases
    - Fix wrong HDF5 attribute when opening files
    - Fix issue with some PLIM methods not deleting the old attribute if it did exist
    - Fix issues with copying scalar attributes
    - Fixed an issue which returned an empty vector for PLI::HDF5::AttributeHandler::getAttributeDimensions when the attribute was an H5S_SCALAR
