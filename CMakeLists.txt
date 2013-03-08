# Require at least cmake 2.6.0
cmake_minimum_required(VERSION 2.6.0 FATAL_ERROR)

# Name of this project
project(QwAnalysis)

# Add the project include directory and put all source files in my_project_sources
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/evio/include
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/include
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/include
  ${CMAKE_CURRENT_SOURCE_DIR}/Tracking/include
  )
file(GLOB my_project_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/*.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/*.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Tracking/src/*.cc
  )
file(GLOB my_project_headers
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/include/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/include/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Tracking/include/*.h
  )

# EVIO library
file(GLOB my_evio_headers
  ${CMAKE_CURRENT_SOURCE_DIR}/evio/include/THaCoda*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/evio/include/evio.h
  )
file(GLOB my_evio_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/evio/src/THaCoda*.C
  ${CMAKE_CURRENT_SOURCE_DIR}/evio/src/evio.C
  ${CMAKE_CURRENT_SOURCE_DIR}/evio/src/swap_util.C
  )
add_library(evio SHARED ${my_evio_sources})
target_link_libraries(evio)

# Exclude those trouble maker files that are not included by default
file(GLOB to_remove ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwOmnivore.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/QwCorrelation.cc)
list(REMOVE_ITEM my_project_sources ${to_remove})


# Local path for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

# Load ROOT and setup include directory
find_package(ROOT REQUIRED)
include_directories(${ROOT_INCLUDE_DIR})

# Load MYSQLPP
find_package(MYSQLPP REQUIRED)
include_directories(${MYSQLPP_INCLUDE_DIR})

# Find Boost libraries
# Though, there seems to be a bug in version 2.8 of CMake's boost
# finding process. As such, we'll disable it for version 2.8
#if(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} EQUAL 2.8)
#  SET( Boost_NO_BOOST_CMAKE ON )
#endif(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} EQUAL 2.8)

find_package(Boost COMPONENTS program_options filesystem system regex REQUIRED)
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIR})

# Make a giant library of all the QwAnalysis stuff, I mean, why not, right? :D
add_library(QwAnalysis SHARED ${my_project_sources})

# Link the ROOT libraries
target_link_libraries(QwAnalysis evio ${ROOT_LIBRARIES} ${MYSQLPP_LIBRARIES}
  ${Boost_LIBRARIES})

# Add the target executables
file(GLOB exefiles "${CMAKE_SOURCE_DIR}/Parity/main/*.cc"
  "${CMAKE_SOURCE_DIR}/Analysis/main/*.cc"
  "${CMAKE_SOURCE_DIR}/Tracking/main/*.cc"
  )

foreach( file ${exefiles} )
  get_filename_component( filename ${file} NAME_WE )
  string( TOLOWER ${filename} filelower )
  add_executable( ${filelower} ${file} )
  target_link_libraries(${filelower} ${ROOT_LIBRARIES} ${MYSQLPP_LIBRARIES}
    ${Boost_LIBRARIES} QwAnalysis evio)
endforeach()

