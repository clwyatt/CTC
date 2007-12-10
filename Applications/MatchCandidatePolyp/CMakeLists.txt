

ADD_EXECUTABLE(MatchCandidatePolyp 
               MatchCandidatePolyp.cxx
               ctcCSVReader.cxx)

ADD_EXECUTABLE(test_MatchCandidatePolyp
               test_MatchCandidatePolyp.cxx)

TARGET_LINK_LIBRARIES(MatchCandidatePolyp ITKIO)

TARGET_LINK_LIBRARIES(test_MatchCandidatePolyp ITKIO)
 
