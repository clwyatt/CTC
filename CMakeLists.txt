# This is the root CTC CMakeLists file.
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
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

# require VTK
FIND_PACKAGE(VTK) 
IF(VTK_FOUND) 
   INCLUDE(${VTK_USE_FILE}) 
ELSE(VTK_FOUND) 
   MESSAGE(FATAL_ERROR "VTK not found. Please set VTK_DIR.") 
ENDIF(VTK_FOUND)

# require FLTK
FIND_PACKAGE(FLTK)
IF(FLTK_FOUND)
   INCLUDE_DIRECTORIES(${FLTK_INCLUDE_DIR})
ENDIF(FLTK_FOUND)

# Slicer3, optional, if found modules will be built
FIND_PACKAGE(Slicer3 QUIET)
IF(Slicer3_FOUND)
   MESSAGE(STATUS "Slicer3 found. Modules enabled.")
   INCLUDE(${Slicer3_USE_FILE})
   slicer3_set_default_install_prefix_for_external_projects()
ENDIF(Slicer3_FOUND)

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
${CTC_SOURCE_DIR}/Utilities/vul
${CTC_SOURCE_DIR}/Utilities/FltkImageViewer
${CTC_SOURCE_DIR}/Code/Common
${CTC_SOURCE_DIR}/Code/IO
${CTC_SOURCE_DIR}/Code/Algorithms/Segmentation
${CTC_SOURCE_DIR}/Code/Algorithms/FeatureExtraction
${CTC_SOURCE_DIR}/Code/Filters
)

# include extra checks
IF(APPLE)
	MESSAGE(STATUS "Apple system: using compiler ENDIAN defines.")
ELSE(APPLE)
       INCLUDE(${CMAKE_ROOT}/Modules/TestBigEndian.cmake)
       TEST_BIG_ENDIAN(CTC_BIG_ENDIAN)
ENDIF(APPLE)

INCLUDE(${CTC_SOURCE_DIR}/CMake/GetSubversionRevision.cmake)

# Configure files with settings for use by the build.
CONFIGURE_FILE(${CTC_SOURCE_DIR}/ctcConfigure.h.in 
               ${CTC_BINARY_DIR}/ctcConfigure.h)
	       
# enable testing
INCLUDE(Dart)
ENABLE_TESTING()

# Dispatch the build into the proper subdirectories.
SUBDIRS(Utilities Code Testing Applications)






