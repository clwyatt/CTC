ADD_EXECUTABLE(fetool 
FeatureExtractionTool.cxx)

TARGET_LINK_LIBRARIES(fetool
ctcvul
CTCIO
CTCFeatureExtraction
ITKStatistics
ITKBasicFilters
ITKCommon
ITKIO)

