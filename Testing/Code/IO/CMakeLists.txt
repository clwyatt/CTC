
# ctcRVFImageIO test
ADD_EXECUTABLE(test_ctcRVFImageIO test_ctcRVFImageIO.cxx ) 

TARGET_LINK_LIBRARIES(test_ctcRVFImageIO CTCIO ITKCommon ITKIO)

ADD_TEST(test_ctcRVFImageIO 
         ${EXECUTABLE_OUTPUT_PATH}/test_ctcRVFImageIO 
         ${CTC_SOURCE_DIR}/Testing/Data/DICOM_short_prone.rvf 
         output_ctcRVFImageIO.rvf)

# ctcCTCImageReader test
ADD_EXECUTABLE(test_ctcCTCImageReader test_ctcCTCImageReader.cxx ) 

TARGET_LINK_LIBRARIES(test_ctcCTCImageReader CTCIO ITKCommon ITKIO)

ADD_TEST(test_up_ctcCTCImageReader 
         ${EXECUTABLE_OUTPUT_PATH}/test_ctcCTCImageReader 
         ${CTC_SOURCE_DIR}/Testing/Data/DICOM_ushort_prone
         output_up_ctcCTCImageReader.vtk)
	 
ADD_TEST(test_sp_ctcCTCImageReader 
         ${EXECUTABLE_OUTPUT_PATH}/test_ctcCTCImageReader 
         ${CTC_SOURCE_DIR}/Testing/Data/DICOM_short_prone
         output_sp_ctcCTCImageReader.vtk)
	 
ADD_TEST(test_us_ctcCTCImageReader 
         ${EXECUTABLE_OUTPUT_PATH}/test_ctcCTCImageReader 
         ${CTC_SOURCE_DIR}/Testing/Data/DICOM_ushort_supine
         output_us_ctcCTCImageReader.vtk)
	 
ADD_TEST(test_ss_ctcCTCImageReader 
         ${EXECUTABLE_OUTPUT_PATH}/test_ctcCTCImageReader 
         ${CTC_SOURCE_DIR}/Testing/Data/DICOM_short_supine
         output_ss_ctcCTCImageReader.vtk)
