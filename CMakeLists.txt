# This is the root CTC CMakeLists file.
CMAKE_MINIMUM_REQUIRED(VERSION 1.8)

PROJECT(CTC)

# VTITK version number
SET(CTC_VERSION_MAJOR "0")
SET(CTC_VERSION_MINOR "1")
SET(CTC_VERSION_PATCH "0")
SET(CTC_VERSION_STRING "${CTC_VERSION_MAJOR}.${CTC_VERSION_MINOR}")

# require ITK
FIND_PACKAGE(ITK) 
IF(ITK_FOUND) 
   INCLUDE(${ITK_USE_FILE}) 
ELSE(ITK_FOUND) 
   MESSAGE(FATAL_ERROR "ITK not found. Please set ITK_DIR.") 
ENDIF(ITK_FOUND)

# Output directories.
SET (LIBRARY_OUTPUT_PATH ${CTC_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all libraries.")
SET (EXECUTABLE_OUTPUT_PATH ${CTC_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all executables.")
MARK_AS_ADVANCED(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)
SET(CTC_LIBRARY_PATH "${LIBRARY_OUTPUT_PATH}")
SET(CTC_EXECUTABLE_PATH "${EXECUTABLE_OUTPUT_PATH}")
SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})

# Put "." in the rpath of all CTC shared libraries.
LINK_DIRECTORIES(.)

# CTC build configuration options.
OPTION(BUILD_SHARED_LIBS "Build CTC with shared libraries." OFF)
SET(CTC_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})

# Create the list of include directories needed for CTC header files.
INCLUDE_DIRECTORIES(
${ITK_INCLUDE_DIRS}
${CTC_BINARY_DIR}
${CTC_SOURCE_DIR}/Code/IO
${CTC_SOURCE_DIR}/Code/Algorithms/Segmentation
)

# include extra checks
INCLUDE(${CTC_SOURCE_DIR}/CMake/GetSubversionRevision.cmake)

# Configure files with settings for use by the build.
CONFIGURE_FILE(${CTC_SOURCE_DIR}/ctcConfigure.h.in 
               ${CTC_BINARY_DIR}/ctcConfigure.h)
	       
# Dispatch the build into the proper subdirectories.
SUBDIRS(Code Testing Applications)






