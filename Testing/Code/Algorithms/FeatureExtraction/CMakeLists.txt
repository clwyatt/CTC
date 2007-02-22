
ADD_EXECUTABLE(test_ctcRawVolumeExtraction test_ctcRawVolumeExtraction.cxx ) 

TARGET_LINK_LIBRARIES(test_ctcRawVolumeExtraction ctcvul 
CTCIO
CTCFeatureExtraction 
ITKBasicFilters 
ITKCommon 
ITKIO)

ADD_TEST(test_ctcRawVolumeExtraction 
         ${EXECUTABLE_OUTPUT_PATH}/test_ctcRawVolumeExtraction
         ${CTC_SOURCE_DIR}/Testing/Data/DICOM_ushort_supine 
         output_ctcRawVolumeExtraction.vtk
         -112 -87 -270
         50)
