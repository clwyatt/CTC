# get the current version of the source for inclusion in
# application information

find_program( SVNVERSION
  svnversion
  /usr/local/bin
  /usr/bin
)

EXEC_PROGRAM(${SVNVERSION} ${CTC_SOURCE_DIR} ARGS "." OUTPUT_VARIABLE CTC_REVISION)
  
#EXECUTE_PROCESS(COMMAND svnversion . 
#	WORKING_DIRECTORY $(CTC_SOURCE_DIR)
#	OUTPUT_VARIABLE CTC_REVISION)
  
SET(CTC_REVISION_STRING "(r${CTC_REVISION})")
