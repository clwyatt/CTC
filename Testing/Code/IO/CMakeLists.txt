
# ctcRVFImageIO test
ADD_EXECUTABLE(test_ctcRVFImageIO test_ctcRVFImageIO.cxx ) 

TARGET_LINK_LIBRARIES(test_ctcRVFImageIO CTCIO ITKCommon ITKIO)

ADD_TEST(test_ctcRVFImageIO 
         ${EXECUTABLE_OUTPUT_PATH}/test_ctcRVFImageIO 
         DICOM_short_prone.rvf 
         output_ctcRVFImageIO.rvf)

# ctcCTCImageReader test
ADD_EXECUTABLE(test_ctcCTCImageReader test_ctcCTCImageReader.cxx ) 

TARGET_LINK_LIBRARIES(test_ctcCTCImageReader CTCIO ctcvul ITKCommon ITKIO)

ADD_TEST(test_up_ctcCTCImageReader 
         ${EXECUTABLE_OUTPUT_PATH}/test_ctcCTCImageReader 
         DICOM_ushort_prone
         output_up_ctcCTCImageReader.vtk
	 -109 -17 -346
	 50)
	 	 
ADD_TEST(test_us_ctcCTCImageReader 
         ${EXECUTABLE_OUTPUT_PATH}/test_ctcCTCImageReader 
         DICOM_ushort_supine
         output_us_ctcCTCImageReader.vtk
	 -112 -87 -270
	 50)

