project(VirtualColon)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# --------------------------------------------------------------------------
# Find Slicer3

if(NOT Slicer3_SOURCE_DIR)
  find_package(Slicer3 REQUIRED)
  include(${Slicer3_USE_FILE})
  slicer3_set_default_install_prefix_for_external_projects()
endif(NOT Slicer3_SOURCE_DIR)

# --------------------------------------------------------------------------
# Include dirs
set(BOOST_LIBRARY_HOME C:/haxu/boost_1_38)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${Slicer3_Libs_INCLUDE_DIRS}
  ${Slicer3_Base_INCLUDE_DIRS}
  ${BOOST_LIBRARY_HOME}
  )

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/vtkSlicerVirtualColonConfigure.h.in 
  ${CMAKE_CURRENT_BINARY_DIR}/vtkSlicerVirtualColonConfigure.h
  )

file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
install(FILES 
  ${headers} 
  "${CMAKE_CURRENT_BINARY_DIR}/vtkSlicerVirtualColonConfigure.h"
  DESTINATION ${Slicer3_INSTALL_MODULES_INCLUDE_DIR}/${PROJECT_NAME} COMPONENT Development
  )

# --------------------------------------------------------------------------
# Sources

set(SlicerVirtualColon_SRCS
  )
  
set(SlicerVirtualColon_NoTCL_SRCS
  vtkSlicerVirtualColonLogic.cxx
  vtkSlicerVirtualColonGUI.cxx
  vtkSlicerVirtualColonNode.cxx
  vtkSlicerViewerInteractorStyle2.cxx
    CSVReader.cpp
    VCPredictSet.cpp
    VCPolypSet.cpp
    VCVoxelSet.cpp
    VCNodeHelper.cpp
    )
  

# Abstract/pure virtual classes

#SET_SOURCE_FILES_PROPERTIES(
#vtkModule.cxx
#PROPERTIES
#ABSTRACT "TRUE"
#)

# Helper classes

#SET_SOURCE_FILES_PROPERTIES(
#vtkModule.cxx
#PROPERTIES
#WRAP_EXCLUDE "TRUE"
#)

# --------------------------------------------------------------------------
# Wrapping

include("${VTK_CMAKE_DIR}/vtkWrapTcl.cmake")
vtk_wrap_tcl3(VirtualColon
  SlicerVirtualColon_TCL_SRCS 
  "${SlicerVirtualColon_SRCS}" "")

#---------------------------------------------------------------------------
# Add Loadable Module support

generatelm(SlicerVirtualColon_SRCS 
  SlicerVirtualColon.txt)

# --------------------------------------------------------------------------
# Build and install the library

set(lib_name VirtualColon)
add_library(${lib_name}
  ${SlicerVirtualColon_SRCS}
  ${SlicerVirtualColon_TCL_SRCS}
  ${SlicerVirtualColon_NoTCL_SRCS}
  )
slicer3_set_modules_output_path(${lib_name})

target_link_libraries(${lib_name}
  ${Slicer3_Libs_LIBRARIES}
  ${Slicer3_Base_LIBRARIES}
  ${KWWidgets_LIBRARIES}
#  ${ITK_LIBRARIES}  
  )

slicer3_install_modules(${lib_name})

# --------------------------------------------------------------------------
# Testing

subdirs(Testing)

if(BUILD_TESTING)
#  subdirs(Testing)
endif(BUILD_TESTING)
