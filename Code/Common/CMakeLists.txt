
INCLUDE_DIRECTORIES(${CTC_SOURCE_DIR}/Code/Common)

SET(CTCCommon_SRCS
  ctcCenterlineSpatialObjectPoint.cxx
  ctcCenterlineSpatialObject.cxx
)

# Add CTCCommon library
ADD_LIBRARY(CTCCommon ${CTCCommon_SRCS})

TARGET_LINK_LIBRARIES(CTCCommon
ITKCommon
)

