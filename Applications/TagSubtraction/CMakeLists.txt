

ADD_EXECUTABLE( TagSubtraction TagSubtraction.cxx )

TARGET_LINK_LIBRARIES ( TagSubtraction
  ctcvul
  CTCIO
  CTCSegmentation
  ITKBasicFilters
  ITKIO
)
