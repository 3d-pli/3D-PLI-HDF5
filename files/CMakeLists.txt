install( DIRECTORY "PLIHDF5"
         EXPORT PLIHDF5Targets
         DESTINATION "${INSTALL_SHARE_DIR}"
         COMPONENT runtime
         FILES_MATCHING PATTERN "*.json" )

