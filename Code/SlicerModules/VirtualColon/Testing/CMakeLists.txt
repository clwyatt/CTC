project(VirtualColon_Testing)

set(BOOST_LIBRARY_INCLUDE /home/haxu/program/boost_1_36_0)
set(Slicer_LIB_DIR /home/haxu/program/Slicer3-build/lib/Slicer/Modules)
set(VC_Lib VirtualColon)
set(VC_Testing_SRC CSVReader_test.cpp)
set(BIN_NAME CSVReader_test)

include_directories(${BOOST_LIBRARY_INCLUDE})

link_directories(${Slicer_LIB_DIR})

add_executable(${BIN_NAME} ${VC_Testing_SRC})

target_link_libraries(${BIN_NAME}
	${VC_Lib}
	)  