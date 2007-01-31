# This project is designed to be built outside the Insight source tree.
PROJECT(ClineFeature2)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)


ADD_EXECUTABLE(ClineFeature2 ClineGenFeaturingMain3.cxx GenerateCline.cxx Dijstra_heap.cxx distancehook2.cxx ClineVoxel.cxx storeLandmark.cxx deformByCLine.cxx)
TARGET_LINK_LIBRARIES(ClineFeature2 ITKNumerics ITKIO ITKStatistics)

