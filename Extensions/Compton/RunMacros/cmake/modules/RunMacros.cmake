# Add the project include directory and put all source files in my_project_sources
set( RunMacros_Dir "$ENV{QWANALYSIS}/Extensions/Compton/RunMacros")
include_directories(${RunMacros_Dir}/include)
file(GLOB my_project_sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)
file(GLOB my_project_headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

# Local path for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH
  "${RunMacros_Dir}/cmake/modules")

# Load ROOT and setup include directory
find_package(ROOT REQUIRED)
include_directories(${ROOT_INCLUDE_DIR})

# Load QwAnalysis and setup include directory
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

# Finally, add the library
add_library(${PROJECT_NAME} SHARED ${my_project_sources})
target_link_libraries(${PROJECT_NAME})

# Install the damn library in the shared libraries directory
INSTALL(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${RunMacros_Dir}/macros.d/)
