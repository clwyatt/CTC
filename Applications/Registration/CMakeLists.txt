

ADD_EXECUTABLE( IntensityRegistration IntensityRegistration.cxx )

TARGET_LINK_LIBRARIES ( IntensityRegistration
  ctcvul
  CTCIO
  ITKBasicFilters
  ITKIO
)
