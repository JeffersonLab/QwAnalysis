# - Finds QwAnalysis instalation
# This module sets up QwAnalysis information
# It defines:
# QWANALYSIS_FOUND          If the QWANALYSIS is found
# QWANALYSIS_INCLUDE_DIR    PATH to the include directory
# QWANALYSIS_LIBRARIES      QwAnalysis libraries

SET(QWANALYSIS_FOUND 0)

find_package( MYSQLPP REQUIRED)

# Find Boost libraries
# Though, there seems to be a bug in version 2.8 of CMake's boost
# finding process. As such, we'll disable it for version 2.8
if(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} EQUAL 2.8)
  SET( Boost_NO_BOOST_CMAKE ON )
endif(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} EQUAL 2.8)

find_package(Boost COMPONENTS filesystem REQUIRED)

# Load ROOT and setup include directory
find_package(ROOT COMPONENTS MathMore REQUIRED)
include_directories(${ROOT_INCLUDE_DIR})



# Look for QWANALYSIS in common paths
IF( DEFINED ENV{QWANALYSIS} AND DEFINED ENV{QW_LIB} )
  SET( QWANALYSIS_INCLUDE_DIR $ENV{QWANALYSIS}/Analysis/include
    $ENV{QWANALYSIS}/Parity/include
    $ENV{QWANALYSIS}/Tracking/include
    ${MYSQLPP_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS})
  FIND_library( _qwanalysis_link_dir Qw
    PATHS
    $ENV{QW_LIB}
    $ENV{QWANALYSIS}/lib
    )
  FIND_library( _qwanalysis_link_dir_evio myevio
    PATHS
    $ENV{QW_LIB}
    $ENV{QWANALYSIS}/lib
    )
  SET(_library_problem 0)
  IF( NOT _qwanalysis_link_dir )
    SET( _library_problem 1 )
  ENDIF( NOT _qwanalysis_link_dir )
  IF( NOT _qwanalysis_link_dir_evio )
  ENDIF( NOT _qwanalysis_link_dir_evio )


  IF( NOT _library_problem )
    SET(QWANALYSIS_FOUND 1)
    SET(QWANALYSIS_LIBRARIES ${_qwanalysis_link_dir}
      ${_qwanalysis_link_dir_evio}
      ${MYSQLPP_LIBRARIES}
      ${Boost_LIBRARIES})
  ELSE( NOT _library_problem )
    SET(QWANALYSIS_FOUND 0)
  ENDIF( NOT _library_problem )

ELSE( DEFINED ENV{QWANALYSIS} AND DEFINED ENV{QW_LIB} )
  SET(QWANALYSIS_FOUND 0)
ENDIF( DEFINED ENV{QWANALYSIS} AND DEFINED ENV{QW_LIB} )

IF( QWANALYSIS_FOUND )
  IF( NOT QwAnalysis_FIND_QUIETLY )
    MESSAGE( STATUS "Found QwAnalysis in $ENV{QWANALYSIS}" )
  ENDIF( NOT QwAnalysis_FIND_QUIETLY )
ELSE( QWANALYSIS_FOUND )
  IF( QwAnalysis_FIND_REQUIRED )
    MESSAGE( SEND_ERROR "Could not find QwAnalysis!" )
  ENDIF (QwAnalysis_FIND_REQUIRED)
ENDIF(QWANALYSIS_FOUND)

# Send variables to cmake
MARK_AS_ADVANCED( QWANALYSIS_LIBRARIES QWANALYSIS_INCLUDE_DIRS )
