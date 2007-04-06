ADD_EXECUTABLE(fetool 
FeatureExtractionTool.cxx 
HistogramFeature.cxx
OrientedVolumeFeature.cxx) 

TARGET_LINK_LIBRARIES(fetool
ctcvul
CTCIO
CTCFeatureExtraction
ITKStatistics
ITKBasicFilters
ITKCommon
ITKIO)

