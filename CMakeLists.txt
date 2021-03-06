CMAKE_MINIMUM_REQUIRED(VERSION 3.0.2)

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

PROJECT(ziptools C)

INCLUDE(CheckFunctionExists)
INCLUDE(CheckIncludeFiles)
INCLUDE(FindPkgConfig)
#INCLUDE(CheckSymbolExists)
INCLUDE(CheckTypeSize)
INCLUDE(GNUInstallDirs)
INCLUDE(FindZLIB)

FIND_PROGRAM(MDOCTOOL NAMES mandoc groff)
IF (MDOCTOOL)
  SET(DOCUMENTATION_FORMAT "mdoc" CACHE STRING "Documentation format")
ELSE()
  FIND_PROGRAM(MANTOOL NAMES nroff)
  IF (MANTOOL)
    SET(DOCUMENTATION_FORMAT "man" CACHE STRING "Documentation format")
  ELSE()
    SET(DOCUMENTATION_FORMAT "html" CACHE STRING "Documentation format")
  ENDIF()
ENDIF()

SET(PACKAGE "ziptools")
SET(PACKAGE_BUGREPORT "ziptools@nih.at")
SET(VERSION_MAJOR "0")
SET(VERSION_MINOR "1")
SET(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}")
SET(PACKAGE_STRING "${PACKAGE} ${VERSION}")

SET(ARCHIVE_NAME ${PACKAGE}-${VERSION})
ADD_CUSTOM_TARGET(dist
  COMMAND git config tar.tar.xz.command "xz -c"
  COMMAND git archive --prefix=${ARCHIVE_NAME}/ -o ${ARCHIVE_NAME}.tar.gz HEAD
  COMMAND git archive --prefix=${ARCHIVE_NAME}/ -o ${ARCHIVE_NAME}.tar.xz HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  )
ADD_CUSTOM_TARGET(distcheck
  COMMAND chmod -R u+w ${ARCHIVE_NAME} ${ARCHIVE_NAME}-build ${ARCHIVE_NAME}-dest 2>/dev/null || true
  COMMAND rm -rf ${ARCHIVE_NAME} ${ARCHIVE_NAME}-build ${ARCHIVE_NAME}-dest
  COMMAND cmake -E tar xf ${ARCHIVE_NAME}.tar.gz
  COMMAND chmod -R u-w ${ARCHIVE_NAME}
  COMMAND mkdir ${ARCHIVE_NAME}-build
  COMMAND mkdir ${ARCHIVE_NAME}-dest
  COMMAND cd ${ARCHIVE_NAME}-build && cmake -DCMAKE_INSTALL_PREFIX=../${ARCHIVE_NAME}-dest ../${ARCHIVE_NAME}
  COMMAND cd ${ARCHIVE_NAME}-build && make -j4
  COMMAND cd ${ARCHIVE_NAME}-build && make test
  COMMAND cd ${ARCHIVE_NAME}-build && make install
#  COMMAND cd ${ARCHIVE_NAME}-build && make uninstall
#  COMMAND if [ `find ${ARCHIVE_NAME}-dest ! -type d | wc -l` -ne 0 ]; then echo leftover files in ${ARCHIVE_NAME}-dest; false; fi
  COMMAND cd ${ARCHIVE_NAME}-build && make clean
  COMMAND chmod -R u+w ${ARCHIVE_NAME} ${ARCHIVE_NAME}-build ${ARCHIVE_NAME}-dest
  COMMAND rm -rf ${ARCHIVE_NAME} ${ARCHIVE_NAME}-build ${ARCHIVE_NAME}-dest
  COMMAND echo "${ARCHIVE_NAME}.tar.gz is ready for distribution."
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  )
ADD_DEPENDENCIES(distcheck dist)

CHECK_FUNCTION_EXISTS(fnmatch HAVE_FNMATCH)
CHECK_FUNCTION_EXISTS(getopt_long HAVE_GETOPT_LONG)
CHECK_FUNCTION_EXISTS(getprogname HAVE_GETPROGNAME)

CHECK_INCLUDE_FILES(stdbool.h HAVE_STDBOOL_H)
CHECK_INCLUDE_FILES(stdint.h HAVE_STDINT_H)

CHECK_TYPE_SIZE(int8_t HAVE_INT8_T)
CHECK_TYPE_SIZE(uint8_t HAVE_UINT8_T)
CHECK_TYPE_SIZE(int16_t HAVE_INT16_T)
CHECK_TYPE_SIZE(uint16_t HAVE_UINT16_T)
CHECK_TYPE_SIZE(int32_t HAVE_INT32_T)
CHECK_TYPE_SIZE(uint32_t HAVE_UINT32_T)
CHECK_TYPE_SIZE(int64_t HAVE_INT64_T)
CHECK_TYPE_SIZE(uint64_t HAVE_UINT64_T)
CHECK_TYPE_SIZE("short" HAVE_SHORT)
CHECK_TYPE_SIZE("int" HAVE_INT)
CHECK_TYPE_SIZE("long" HAVE_LONG)
CHECK_TYPE_SIZE("long long" HAVE_LONG_LONG)
CHECK_TYPE_SIZE("off_t" HAVE_SIZEOF_OFF_T)

INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIRS})

PKG_CHECK_MODULES(LIBZIP REQUIRED libzip)
INCLUDE_DIRECTORIES(${LIBZIP_INCLUDE_DIRS})
ADD_COMPILE_OPTIONS(${LIBZIP_CFLAGS_OTHER})
PKG_GET_VARIABLE(ZIPCMP libzip zipcmp)

# Instead of using `link_directories`, collect absolute paths to libraries.
SET(MAIN_LIBS_ABSOLUTE ${ZLIB_LIBRARIES})
FOREACH(libname LIBZIP SQLITE3)
  FOREACH(lib ${${libname}_LIBRARIES})
    # Choose name of variable, which will contain result of `find_library`
    # for specific library.
    SET(var_name MAIN_${lib}_ABS)
    # Search library under dirs, returned by pkg-config.
    FIND_LIBRARY(${var_name} ${lib} ${${libname}_LIBRARY_DIRS})
    LIST(APPEND MAIN_LIBS_ABSOLUTE ${${var_name}})
  ENDFOREACH()
ENDFOREACH()
SET(SUPPORT_LIBS_ABSOLUTE)
FOREACH(lib ${LIBXML2_LIBRARIES})
  # Choose name of variable, which will contain result of `find_library`
  # for specific library.
  SET(var_name MAIN_${lib}_ABS)
  # Search library under dirs, returned by pkg-config.
  FIND_LIBRARY(${var_name} ${lib} ${${libname}_LIBRARY_DIRS})
  LIST(APPEND SUPPORT_LIBS_ABSOLUTE ${${var_name}})
ENDFOREACH()

ADD_DEFINITIONS("-DHAVE_CONFIG_H")

# install with rpath
IF(NOT CMAKE_SYSTEM_NAME MATCHES Linux)
  SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
ENDIF()

# Testing
ENABLE_TESTING()

# Targets
#ADD_SUBDIRECTORY(docs)
ADD_SUBDIRECTORY(src)
#ADD_SUBDIRECTORY(regress)

# write out config file
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/cmake-config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

# for tests

#SET(srcdir ${CMAKE_CURRENT_SOURCE_DIR}/regress)
#SET(abs_srcdir ${CMAKE_CURRENT_SOURCE_DIR}/regress)
#SET(top_builddir ${CMAKE_BINARY_DIR})

#CONFIGURE_FILE(regress/runtest.in ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/runtest @ONLY)
#FILE(COPY ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/runtest
#  DESTINATION ${CMAKE_BINARY_DIR}/regress
#  FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
#)
