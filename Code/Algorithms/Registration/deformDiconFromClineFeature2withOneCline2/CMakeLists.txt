# This project is designed to be built outside the Insight source tree.
PROJECT(YunaDiconClineFeat)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)


ADD_EXECUTABLE(YunaDiconClineFeat YunaMain.cxx YunaGenerateCline.cxx YunaDijstra_heap.cxx YunaClineVoxel.cxx YunaStoreLandmark.cxx YunaDeformByCLine.cxx)
TARGET_LINK_LIBRARIES(YunaDiconClineFeat ITKNumerics ITKIO ITKStatistics)

