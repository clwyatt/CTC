

ADD_EXECUTABLE( SegmentColon SegmentColon.cxx )

TARGET_LINK_LIBRARIES ( SegmentColon
  ctcvul
  CTCIO
  CTCSegmentation
  ITKBasicFilters
  ITKIO
)
