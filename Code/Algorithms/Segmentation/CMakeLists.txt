
INCLUDE_DIRECTORIES(${CTC_SOURCE_DIR}/Code/Common)

SET(CTCSegmentation_SRCS
  ctcSegmentColonFilter.cxx
  ctcSegmentColonWithContrastFilter.cxx
  ctcSegmentProjectionFilter.cxx
  ctcDetectAndSegmentPolypsNappi02Filter.cxx
)

# Add CTCSegmentation library
ADD_LIBRARY(CTCSegmentation ${CTCSegmentation_SRCS})

TARGET_LINK_LIBRARIES(CTCSegmentation
ITKCommon
)

