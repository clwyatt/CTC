
ADD_EXECUTABLE(test_ctcRawVolumeExtraction test_ctcRawVolumeExtraction.cxx ) 

TARGET_LINK_LIBRARIES(test_ctcRawVolumeExtraction ctcvul 
CTCIO
CTCFeatureExtraction 
ITKBasicFilters 
ITKCommon 
ITKIO)

ADD_TEST(test_ctcRawVolumeExtraction 
         ${EXECUTABLE_OUTPUT_PATH}/test_ctcRawVolumeExtraction
         DICOM_ushort_supine 
         output_ctcRawVolumeExtraction.vtk
         -112 -87 -270
         50)

ADD_EXECUTABLE(test_ctcPrincipleCurvatureExtraction
               test_ctcPrincipleCurvatureExtraction.cxx ) 

TARGET_LINK_LIBRARIES(test_ctcPrincipleCurvatureExtraction 
ctcvul 
CTCIO
CTCSegmentation
CTCFeatureExtraction 
ITKBasicFilters 
ITKCommon 
ITKIO)

ADD_TEST(test_ctcPrincipleCurvatureExtraction 
         ${EXECUTABLE_OUTPUT_PATH}/test_ctcPrincipleCurvatureExtraction
         DICOM_short_supine 
         output_ctcPrincipleCurvatureExtraction.vtk)
