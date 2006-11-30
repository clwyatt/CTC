
ADD_EXECUTABLE(test_ctcRVFImageIO test_ctcRVFImageIO.cxx ) 

TARGET_LINK_LIBRARIES(test_ctcRVFImageIO CTCIO ITKCommon ITKIO)

ADD_TEST(test_ctcRVFImageIO ${EXECUTABLE_OUTPUT_PATH}/test_ctcRVFImageIO 
${CTC_SOURCE_DIR}/Testing/Data/DICOM_short_prone.rvf output_ctcRVFImageIO.rvf)
