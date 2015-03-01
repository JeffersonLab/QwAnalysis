# Require at least cmake 2.6.0
cmake_minimum_required(VERSION 2.6.0 FATAL_ERROR)

# Name of this project
project(QwAnalysis)

# Add the project include directory
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/evio/include
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/include
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/include
  ${CMAKE_CURRENT_SOURCE_DIR}/Tracking/include
  )

# Put all QwAnalysis files in my_project_sources
file(GLOB my_project_headers
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/include/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/include/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Tracking/include/*.h
  )
file(GLOB my_project_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/*.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/*.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Tracking/src/*.cc
  )

# Put all evio files in my_project_sources (exclude THaEtClient)
file(GLOB my_evio_headers
  ${CMAKE_CURRENT_SOURCE_DIR}/evio/include/THaCoda*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/evio/include/evio.h
  )
file(GLOB my_evio_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/evio/src/THaCoda*.C
  ${CMAKE_CURRENT_SOURCE_DIR}/evio/src/evio.C
  ${CMAKE_CURRENT_SOURCE_DIR}/evio/src/swap_util.C
  )

# Local path for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

# Load ROOT and setup include directory
find_package(ROOT REQUIRED)
include_directories(${ROOT_INCLUDE_DIR})
set(ROOT_LIBRARIES ${ROOT_LIBRARIES} -lGui)

# Load MYSQLPP
find_package(MYSQLPP REQUIRED)
include_directories(${MYSQLPP_INCLUDE_DIR})

# Find Boost libraries
# Though, there seems to be a bug in version 2.8 of CMake's boost
# finding process. As such, we'll disable it for version 2.8
if(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} EQUAL 2.8)
  SET( Boost_NO_BOOST_CMAKE ON )
endif(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} EQUAL 2.8)

find_package(Boost COMPONENTS program_options filesystem system regex REQUIRED)
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIR})

# Create dictionaries for headers that contain "ClassDef" if ROOT version < 5.90
if(ROOT_VERSION VERSION_LESS 5.90)
message("Generating evio dictionaries for ROOT ${ROOT_VERSION}...")
# Loop over all evio header files
foreach(file ${my_evio_headers})
  # Find the header files that have uncommented ClassDef lines
  execute_process(COMMAND grep -l "[^/]ClassDef" ${file} OUTPUT_VARIABLE result)
  if(result)
    # Get path and filename without extension
    get_filename_component(file_path ${file} PATH)
    get_filename_component(file_name ${file} NAME_WE)
    # Add dictionary target with possible LinkDef file
    set(dict ${CMAKE_CURRENT_BINARY_DIR}/${file_name}Dict)
    ROOT_GENERATE_DICTIONARY(
      ${dict}
      ${file}
      OPTIONS -p)
    # Add dictionary to evio sources
    set(my_evio_sources ${my_evio_sources} ${dict}.cxx)
  endif()
endforeach()
else()
message("No evio dictionaries needed for ROOT ${ROOT_VERSION}.")
endif()

# Add evio library
add_library(evio SHARED ${my_evio_sources})
target_link_libraries(evio)


# Create dictionaries for headers that contain "ClassDef" if ROOT version < 5.90
if(ROOT_VERSION VERSION_LESS 5.90)
message("Generating QwAnalysis dictionaries for ROOT ${ROOT_VERSION}...")
# Loop over all project header files
foreach(file ${my_project_headers})
  # Find the header files that have uncommented ClassDef lines
  execute_process(COMMAND grep -l "[^/]ClassDef" ${file} OUTPUT_VARIABLE result)
  if(result)
    # Get path and filename without extension
    get_filename_component(file_path ${file} PATH)
    get_filename_component(file_name ${file} NAME_WE)
    # Add dictionary target with possible LinkDef file
    set(dict ${CMAKE_CURRENT_BINARY_DIR}/${file_name}Dict)
    set(linkdef ${file_path}/${file_name}LinkDef.h)
    if(EXISTS ${linkdef})
      ROOT_GENERATE_DICTIONARY(
        ${dict}
        ${file}
        LINKDEF ${linkdef}
        OPTIONS -p)
    else()
      ROOT_GENERATE_DICTIONARY(
        ${dict}
        ${file}
        OPTIONS -p)
    endif()
    # Add dictionary to project sources
    set(my_project_sources ${my_project_sources} ${dict}.cxx)
  endif()
endforeach()
else()
message("No QwAnalysis dictionaries needed for ROOT ${ROOT_VERSION}.")
endif()

# Make a giant library of all the QwAnalysis stuff, I mean, why not, right? :D
add_library(QwAnalysis SHARED ${my_project_sources})


# Link the ROOT libraries
target_link_libraries(QwAnalysis evio ${ROOT_LIBRARIES} ${MYSQLPP_LIBRARIES}
  ${Boost_LIBRARIES})

# Add the target executables
file(GLOB exefiles
  "${CMAKE_SOURCE_DIR}/Parity/main/*.cc"
  "${CMAKE_SOURCE_DIR}/Analysis/main/*.cc"
  "${CMAKE_SOURCE_DIR}/Tracking/main/*.cc"
  )
foreach( file ${exefiles} )
  get_filename_component( filename ${file} NAME_WE )
  string( TOLOWER ${filename} filelower )
  add_executable( ${filelower} ${file} )
  target_link_libraries(
    ${filelower}
    ${ROOT_LIBRARIES} ${MYSQLPP_LIBRARIES} ${Boost_LIBRARIES} QwAnalysis evio
    )
endforeach()

