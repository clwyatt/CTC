
INCLUDE_DIRECTORIES(${CTC_SOURCE_DIR}/Code/Common)

SET(CTCFeatureExtraction_SRCS
  ctcRawVolumeExtraction.cxx
)

# Add CTCFeatureExtraction library
ADD_LIBRARY(CTCFeatureExtraction ${CTCFeatureExtraction_SRCS})

TARGET_LINK_LIBRARIES(CTCFeatureExtraction
ITKCommon
)

