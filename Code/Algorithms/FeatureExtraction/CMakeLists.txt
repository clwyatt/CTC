
INCLUDE_DIRECTORIES(${CTC_SOURCE_DIR}/Code/Common)

SET(CTCFeatureExtraction_SRCS
  ctcRawVolumeExtraction.cxx
  ctcPrincipleCurvatureExtraction.cxx
  ctcFeatureExtraction.cxx
  MAT4Converter.cpp
  StringtoDouble.cpp
)

# Add CTCFeatureExtraction library
ADD_LIBRARY(CTCFeatureExtraction ${CTCFeatureExtraction_SRCS})

TARGET_LINK_LIBRARIES(CTCFeatureExtraction
ITKCommon
)


