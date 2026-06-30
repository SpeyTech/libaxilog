#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "axilog::axilog" for configuration "Release"
set_property(TARGET axilog::axilog APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(axilog::axilog PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libaxilog.a"
  )

list(APPEND _cmake_import_check_targets axilog::axilog )
list(APPEND _cmake_import_check_files_for_axilog::axilog "${_IMPORT_PREFIX}/lib/libaxilog.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
