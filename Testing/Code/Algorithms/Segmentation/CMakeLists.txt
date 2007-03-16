
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
