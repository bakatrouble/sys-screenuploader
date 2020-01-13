# Tries to find libtwili
# Once done, this will define:
# > LIBTWILI_FOUND - The system has libtwili
# > LIBTWILI_INCLUDE_DIRS - The libtwili include directories
# > LIBTWILI_LIBRARIES - The libtwili libraries required for using it
#
# It also adds an imported target named `switch::libtwili`.

if (NOT SWITCH)
    cmake_panic("This helper can only be used if you are using the Switch toolchain file.")
endif ()

set(LIBTWILI_PATHS $ENV{LIBTWILI} libtwili ${LIBTWILI} ${PORTLIBS})

find_path(LIBTWILI_INCLUDE_DIR twili.h
        PATHS ${LIBTWILI_PATHS}
        PATH_SUFFIXES include)

find_library(LIBTWILI_LIBRARY NAMES libtwili.a
        PATHS ${LIBTWILI_PATHS}
        PATH_SUFFIXES lib)

set(LIBTWILI_INCLUDE_DIRS ${LIBTWILI_INCLUDE_DIR})
set(LIBTWILI_LIBRARIES ${LIBTWILI_LIBRARY})

# Handle the QUIETLY and REQUIRED arguments and set LIBTWILI_FOUND to TRUE if all above variables are TRUE.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LIBTWILI DEFAULT_MSG
        LIBTWILI_INCLUDE_DIR LIBTWILI_LIBRARY)

mark_as_advanced(LIBTWILI_INCLUDE_DIR LIBTWILI_LIBRARY)
if (LIBTWILI_FOUND)
    set(LIBTWILI ${LIBTWILI_INCLUDE_DIR}/..)
    cmake_info("Setting LIBTWILI to ${LIBTWILI}")

    add_library(switch::libtwili STATIC IMPORTED GLOBAL)
    set_target_properties(switch::libtwili PROPERTIES
            IMPORTED_LOCATION ${LIBTWILI_LIBRARY}
            INTERFACE_INCLUDE_DIRECTORIES ${LIBTWILI_INCLUDE_DIR})
endif ()