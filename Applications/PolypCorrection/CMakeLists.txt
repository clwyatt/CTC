

SET(PolypCorrection_GUI_SRCS
  PolypCorrectionGUI.fl
  )

FLTK_WRAP_UI( PolypCorrection ${PolypCorrection_GUI_SRCS} )

SET ( PolypCorrection_SRCS
  PolypCorrection.cxx
  ${PolypCorrection_FLTK_UI_SRCS}
  )

ADD_EXECUTABLE( PolypCorrection ${PolypCorrection_SRCS} )

TARGET_LINK_LIBRARIES (PolypCorrection
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
