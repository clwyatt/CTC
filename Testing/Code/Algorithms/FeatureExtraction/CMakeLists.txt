
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
         supine.vtk
         output_ctcPrincipleCurvatureExtraction.txt)
set_tests_properties(test_ctcPrincipleCurvatureExtraction PROPERTIES TIMEOUT 3600)

ADD_EXECUTABLE(test_ctcMAT4Converter test_ctcMAT4Converter.cxx)             

TARGET_LINK_LIBRARIES(test_ctcMAT4Converter 
ctcvul 
CTCIO
CTCSegmentation
CTCFeatureExtraction 
ITKBasicFilters 
ITKCommon 
ITKIO)

ADD_TEST(test_ctcMAT4Converter 
         SupineData.vtk
         SupineSegmented.vtk 
         output_ctcFeatureExtraction.vtk) 



ADD_EXECUTABLE(test_ctcFeatureExtraction test_ctcFeatureExtraction.cxx)             

TARGET_LINK_LIBRARIES(test_ctcFeatureExtraction 
ctcvul 
CTCIO
CTCSegmentation
CTCFeatureExtraction 
ITKBasicFilters 
ITKCommon 
ITKIO)

ADD_TEST(test_ctcFeatureExtraction
         SupineData.vtk
         SupineSegmented.vtk 
         output_ctcFeatureExtraction.vtk)        
