# Require at least cmake 2.6.0
cmake_minimum_required(VERSION 2.6.0 FATAL_ERROR)

# Name of this project
project(RunMacros_Project)

# Add the project include directory and put all source files in my_project_sources
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB my_project_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)
file(GLOB my_project_headers ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

# Local path for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

# Load ROOT and setup include directory
find_package(ROOT REQUIRED)
include_directories(${ROOT_INCLUDE_DIR})

# Load QWANALYSIS and include directories
find_package(QwAnalysis REQUIRED)
include_directories(${QWANALYSIS_INCLUDE_DIR})

# Find SQLite3
find_package(Sqlite3 REQUIRED)
include_directories(${SQLITE3_INCLUDE_DIRS})

# Find Boost libraries
# Though, there seems to be a bug in version 2.8 of CMake's boost
# finding process. As such, we'll disable it for version 2.8
if(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} EQUAL 2.8)
  SET( Boost_NO_BOOST_CMAKE ON )
endif(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} EQUAL 2.8)

find_package(Boost COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIR})

# Add the target executables
add_executable(RunMacros RunMacros.cc ${my_project_sources} ${my_project_headers})

# Link the ROOT libraries
target_link_libraries(RunMacros ${ROOT_LIBRARIES} ${SQLITE3_LIBRARIES}
  ${QWANALYSIS_LIBRARIES} ${Boost_LIBRARIES})
