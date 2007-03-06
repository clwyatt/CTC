
ADD_EXECUTABLE(test_ctcGaborImageFilter test_ctcGaborImageFilter.cxx) 

TARGET_LINK_LIBRARIES(test_ctcGaborImageFilter 
                      ctcvul ITKBasicFilters ITKCommon ITKIO)
