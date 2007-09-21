SET(fltkImageViewer_GUI_SRCS
 fltkimageViewerGUI.fl
)

FLTK_WRAP_UI( fltkImageViewer ${fltkImageViewer_GUI_SRCS} )

SET ( ManualSegment_SRCS
  ManualSegmentCorrection.cxx
  ${fltkImageViewer_FLTK_UI_SRCS}	
  )

ADD_EXECUTABLE( ManualCorrect ${ManualSegment_SRCS} )

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
