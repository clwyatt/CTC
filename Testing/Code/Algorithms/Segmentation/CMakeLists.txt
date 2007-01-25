
ADD_EXECUTABLE(test_ctcSegmentColonFilter test_ctcSegmentColonFilter.cxx ) 

TARGET_LINK_LIBRARIES(test_ctcSegmentColonFilter ctcvul CTCIO ITKBasicFilters ITKCommon ITKIO)

ADD_TEST(test_ctcSegmentColonFilter ${EXECUTABLE_OUTPUT_PATH}/test_ctcSegmentColonFilter
${CTC_SOURCE_DIR}/Testing/Data/DICOM_short_supine.rvf output_supine_ctcSegmentColon.vtk)
