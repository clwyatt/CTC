SET(ManualCorrect_GUI_SRCS
 fltkimageViewerGUI.fl
)

FLTK_WRAP_UI(ManualCorrect ${ManualCorrect_GUI_SRCS} )

SET ( ManualCorrect_SRCS
  ManualSegmentCorrection.cxx
  ${ManualCorrect_FLTK_UI_SRCS}	
  )

ADD_EXECUTABLE( ManualCorrect ${ManualCorrect_SRCS} )

TARGET_LINK_LIBRARIES (ManualCorrect
  ctcvul
  CTCIO
  CTCFeatureExtraction
  ITKBasicFilters
  ITKIO
  FltkImageViewer
  fltk_forms
  fltk_gl
  fltk_images
  fltk_jpeg
  fltk_png
  fltk_zlib
)
