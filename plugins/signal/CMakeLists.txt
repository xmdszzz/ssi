#-----------------------------------------------------------------------------
# Initialize project.

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	if(NOT DEFINED WITH_IN_SOURCE_BUILD)
		message(FATAL_ERROR
			"CMake generation for openssi is not allowed within the source directory!"
			"\n Remove the CMakeCache.txt file and try again from another folder, e.g.:"
			"\n "
			"\n rm CMakeCache.txt"
			"\n cd .."
			"\n mkdir cmake-make"
			"\n cd cmake-make"
			"\n cmake ../core"
			"\n "
			"\n Alternately define WITH_IN_SOURCE_BUILD to force this option (not recommended!)"
		)		
	endif()
endif()

#http://stackoverflow.com/questions/25609692/how-to-add-source-files-in-another-folder
#file(GLOB Dir1_Sources RELATIVE "Dir1" "*.cpp")
#http://www.opentissue.org/mediawiki/index.php/Using_CMake
cmake_minimum_required(VERSION 2.8)

project(ssisignal)

IF(SSI_GUI STREQUAL "SDL")
	IF(UNIX)
		INCLUDE(FindPkgConfig)
		pkg_check_modules(CAIRO REQUIRED cairo)
		pkg_check_modules(SDL2 REQUIRED sdl2)
        endif(UNIX)
        add_subdirectory(test)
        add_dependencies(ssisignal_test ssi ssisignal)
endif(SSI_GUI STREQUAL "SDL")


include_directories(
					include
					include/ioput
					include/ioput/socket
					../../core/include/ioput/socket
					../../core/include/ioput
					../../core/include
                                        ../../libs/build/kissfft/include
                                        ../../libs/build/ExpressionParser/include/
					${CAIRO_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS}
					)
if(ANDROID)

else(ANDROID)

endif(ANDROID)

set(COMMON_SRC_FILES


source/Bundle.cpp
source/Butfilt.cpp
source/ConvPower.cpp
source/Derivative.cpp
source/DownSample.cpp
source/Energy.cpp
source/ExportMain.cpp
source/Expression.cpp
source/FFTfeat.cpp
#source/FilterTools.cpp
source/Functionals.cpp
source/FunctionalsEventSender.cpp
source/Gate.cpp
source/IIR.cpp
source/Integral.cpp
source/Intensity.cpp
source/Limits.cpp
source/MFCC.cpp
source/Mean.cpp
source/Multiply.cpp
source/MvgAvgVar.cpp
source/MvgConDiv.cpp
source/MvgDrvtv.cpp
source/MvgMedian.cpp
source/MvgMedianHelper.cpp
source/MvgMinMax.cpp
source/MvgNorm.cpp
source/MvgPeakGate.cpp
source/Noise.cpp
source/Normalize.cpp
#source/Operator0.cpp
#source/Operator1.cpp
source/Pulse.cpp
source/Spectrogram.cpp
source/Statistics.cpp
source/Sum.cpp
source/Relative.cpp

)


set(SRC_FILES ${COMMON_SRC_FILES} )

#find_library(OPENSSI_CORE_LIBRARY NAMES openssi-core HINTS ../../cmake-core/corelib/Debug/ ../../cmake-core/corelib/Release/)
IF(MINGW)
  find_library (MINGW_WSOCKET "wsock32" HINTS ${MINGWLIB} )
  find_library (MINGW_WMM "winmm" HINTS ${MINGWLIB})
  find_library (MINGW_WSOCKET2 "ws2_32" HINTS ${MINGWLIB} )
  find_library (MINGW_PTHREAD "pthread" HINTS ${MINGWLIB} )
  	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -ggdb" )
  ELSEIF(UNIX)
  SET(MINGW_WSOCKET "")
  SET(MINGW_WSOCKET2 "")
  SET(MINGW_WMM "")
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SSI_CFLAG}" )
  set( CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${SSI_LD_FLAG}" )
  set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${SSI_LD_FLAG}" )
  set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${SSI_LD_FLAG}" )
    if(ANDROID)
  set(MINGW_PTHREAD "")
  else(ANDROID)
  find_library (MINGW_PTHREAD "pthread" HINTS "/usr/lib" )
  endif(ANDROID)
ELSE(MINGW)
  SET(MINGW_WSOCKET "")
  SET(MINGW_WSOCKET2 "")
  SET(MINGW_WMM "")
   SET(MINGW_PTHREAD "")
ENDIF(MINGW)




add_library(ssisignal SHARED ${SRC_FILES} )
target_link_libraries(ssisignal ssi kissFFT ExpressionParser ${MINGW_WSOCKET} ${MINGW_WSOCKET2}  ${MINGW_WMM} ${MINGW_PTHREAD} ${CAIRO_LIBRARIES} ${SDL2_LIBRARIES} -dl)
#add_executable(myapp main.c)

#rename targets if debug
  if(ANDROID)

  else(ANDROID)
set_target_properties(ssisignal PROPERTIES DEBUG_POSTFIX "d" PREFIX "")
  endif(ANDROID)

#install target to ssi install path set in base directory
install(TARGETS ssisignal DESTINATION ${SSI_INSTALL}/${SSI_PLATFORM}/${SSI_COMPILER})
