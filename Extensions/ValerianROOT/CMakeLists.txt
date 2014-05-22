# Require at least cmake 2.6.0
cmake_minimum_required(VERSION 2.6.0 FATAL_ERROR)

# Name of this project
project(ValerianRoot_Project)

# Add the project include directory and put all source files in my_project_sources
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB my_project_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)
file(GLOB my_project_headers ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
file(GLOB my_cmake_modules ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/*.cmake
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/COPYING-CMAKE-SCRIPTS )
set(my_dic_sources "")


# Local path for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

# Load ROOT and setup include directory
find_package(ROOT REQUIRED)
include_directories(${ROOT_INCLUDE_DIR})
# Create dictionary for headers if ROOT version < 5.90
if(ROOT_VERSION VERSION_LESS 5.90)
#message("Generating dictionaries for ROOT ${ROOT_VERSION}...")
    set(ROOTPWA_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/Dictionary.cxx)
  root_generate_dictionary(
    "${my_project_headers}"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/linkdef/ValerianLinkDef.h"
    "${ROOTPWA_DICTIONARY}"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )
    set(my_dic_sources ${my_dic_sources} ${ROOTPWA_DICTIONARY})
else()
message("No dictionaries needed for ROOT ${ROOT_VERSION}.")
endif()

#if(ROOT_VERSION VERSION_LESS 5.90)
#message("Generating dictionaries for ROOT ${ROOT_VERSION}...")
#foreach(file ${my_project_headers})
#    get_filename_component(file_name ${file} NAME_WE)
#    set(ROOTPWA_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/${file_name}Dictionary.cxx)
#  root_generate_dictionary(
#    "${file}"
#    "${CMAKE_CURRENT_SOURCE_DIR}/include/ValerianLinkDef.h"
#    "${ROOTPWA_DICTIONARY}"
#    "${CMAKE_CURRENT_SOURCE_DIR}/include"
#    )
#    set(my_dic_sources ${my_dic_sources} ${ROOTPWA_DICTIONARY})
#endforeach()
#else()
#message("No dictionaries needed for ROOT ${ROOT_VERSION}.")
#endif()



message("Compiling the following dictionary files: ${my_dic_sources}")
# Add evio library
add_library(ValerianROOTDic SHARED ${my_dic_sources})

message("\nYou are making ValerianROOTTreeMaker... unknowen time to metamorphosis\n")

# Add the target executables
add_executable(ValerianROOTTreeMaker ValerianROOTTreeMaker.cc ${my_project_sources} ${my_dic_sources}
  ${my_project_headers})

message("\nYou are makeing ValerianROOT... unknown time to hatching\n")

add_executable(ValerianROOT ValerianROOT.cc ${my_project_sources} ${my_dic_sources}
  ${my_project_headers})

# Link the ROOT libraries
target_link_libraries(ValerianROOTTreeMaker ${ROOT_LIBRARIES} )
target_link_libraries(ValerianROOT ${ROOT_LIBRARIES})

message("\nIf all went well there is a little bird and butterfly around somewhere...\n take a break and go find it :)\n")
