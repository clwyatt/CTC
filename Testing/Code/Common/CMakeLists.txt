
# ctcCenterlineSpatialObject test
ADD_EXECUTABLE(test_ctcCenterlineSpatialObject test_ctcCenterlineSpatialObject.cxx ) 

TARGET_LINK_LIBRARIES(test_ctcCenterlineSpatialObject CTCIO CTCCommon ITKCommon ITKIO ctcvul)

ADD_TEST(test_ctcCenterlineSpatialObject 
         ${EXECUTABLE_OUTPUT_PATH}/test_ctcCenterlineSpatialObject
         test_ctcCenterlineSpatialObject_OUTPUT)



