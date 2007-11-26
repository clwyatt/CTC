# This project is designed to be built outside the Insight source tree.
PROJECT(InsidePositiveDistMap3D)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)


ADD_EXECUTABLE(InsidePositiveDistMap3D SignedDanielssonDistanceMapImageFilter.cxx )
TARGET_LINK_LIBRARIES(InsidePositiveDistMap3D ITKNumerics ITKIO ITKStatistics)
#TARGET_LINK_LIBRARIES(DemonTst ITKIO ITKStatistics)
