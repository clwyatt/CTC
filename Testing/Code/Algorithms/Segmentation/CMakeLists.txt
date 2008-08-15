##########################################################################
ADD_EXECUTABLE(test_ctcSegmentColonFilter test_ctcSegmentColonFilter.cxx ) 

TARGET_LINK_LIBRARIES(test_ctcSegmentColonFilter 
ctcvul 
CTCIO 
CTCSegmentation 
ITKBasicFilters 
ITKCommon 
ITKIO
vtkIO)

ADD_TEST(test_ctcSegmentColonFilter 
${EXECUTABLE_OUTPUT_PATH}/test_ctcSegmentColonFilter
DICOM_ushort_prone 
output_ctcSegmentColon)

set_tests_properties (test_ctcSegmentColonFilter PROPERTIES TIMEOUT 3600)

#########################################################################
ADD_EXECUTABLE(test_ctcSegmentColonWithContrastFilter test_ctcSegmentColonWithContrastFilter.cxx ) 

TARGET_LINK_LIBRARIES(test_ctcSegmentColonWithContrastFilter 
ctcvul 
CTCIO 
CTCSegmentation 
ITKBasicFilters 
ITKCommon 
ITKIO
vtkIO)

ADD_TEST(test_ctcSegmentColonWithContrastFilter 
${EXECUTABLE_OUTPUT_PATH}/test_ctcSegmentColonWithContrastFilter
DICOM_short_supine 
output_ctcSegmentColonWithContrast)

set_tests_properties (test_ctcSegmentColonWithContrastFilter PROPERTIES TIMEOUT 5400)

##########################################################################
ADD_EXECUTABLE(test_ctcSegmentProjectionFilter test_ctcSegmentProjectionFilter.cxx ) 

TARGET_LINK_LIBRARIES(test_ctcSegmentProjectionFilter 
ctcvul 
CTCIO 
CTCSegmentation 
ITKBasicFilters 
ITKCommon 
ITKIO)

ADD_TEST(test_ctcSegmentProjectionFilter 
${EXECUTABLE_OUTPUT_PATH}/test_ctcSegmentProjectionFilter
supine.vtk
output_ctcSegmentProjection.vtk)

