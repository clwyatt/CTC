

SET ( PolypCoordinateToIndex_SRCS
  PolypCoordinateToIndex.cxx
  )

ADD_EXECUTABLE( PolypCoordinateToIndex ${PolypCoordinateToIndex_SRCS} )

TARGET_LINK_LIBRARIES (PolypCoordinateToIndex
  ctcvul
  CTCIO
  CTCFeatureExtraction
  ITKBasicFilters
  ITKIO
)
