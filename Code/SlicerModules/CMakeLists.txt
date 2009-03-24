# Code/SlicerModules/CMakeLists.txt

IF(Slicer3_FOUND)

  # The segmentation Command Line Module
  SET(CLP ColonSegmenter)
  SET(${CLP}_SOURCE ${CLP}.cxx)
  generateclp(${CLP}_SOURCE ${CLP}.xml)

  add_executable(${CLP} ${${CLP}_SOURCE})
  slicer3_set_plugins_output_path(${CLP})
  target_link_libraries(${CLP} ${ITK_LIBRARIES})

  slicer3_install_plugins(${CLP})

ENDIF(Slicer3_FOUND)


