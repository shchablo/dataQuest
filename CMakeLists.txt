################################################################################
# CMakeLists.txt for dataQuest project.
# Shchablo Konstantin (Shchablo@gmail.com)
################################################################################

cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

project(dq LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_PLATFORM_INDEPENDENT_CODE ON)


# MODULE -----------------------------------------------------------------------

  list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/modules)
  
  ## CERN_ROOT
  #list(APPEND CMAKE_MODULE_PATH $ENV{ROOTSYS}/cmake/)
  find_package(ROOT CONFIG REQUIRED)
  set(ROOT_DIR $ENV{ROOTSYS})
  include("${ROOT_DIR}/cmake/modules/RootNewMacros.cmake")  
  
  include("${ROOT_USE_FILE}")
  
  #include_directories(${ROOT_USE_FILE})
  #include_directories(${ROOT_INCLUDE_DIR})
  ROOT_GENERATE_DICTIONARY(G__RoPetiEvent
    ${CMAKE_CURRENT_SOURCE_DIR}/include/RoPetiEvent/RoPetiEvent.h LINKDEF
    ${CMAKE_CURRENT_SOURCE_DIR}/include/RoPetiEvent/RoPetiEventLinkDef.h) 
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/libs)
  add_library(RoPetiEvent SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RoPetiEvent/RoPetiEvent.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/include/RoPetiEvent/RoPetiEvent.h
    G__RoPetiEvent.cxx)
  ##
  target_include_directories(RoPetiEvent PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
  target_link_libraries(RoPetiEvent PUBLIC ROOT::Core)
  
  ## ZLIB
    find_package(ZLIB REQUIRED)
    include_directories(${ZLIB_IBCLUDE_DIRS})
  ##
#-------------------------------------------------------------------------------

# PROJECT INCLUDES -------------------------------------------------------------
  include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
  ## dq 
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
    ### RoPetiEvent
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/RoPetiEvent)
    ### InOutData
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/InOutData)
    ###
    ### DqROOT
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/MyROOT)
    ###
    ### Wee2UI (Petiroc2UI)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/Wee2UI)
    ###
    ### LyPeti (Lyon Petiroc chip)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/LyPeti)
    ###
    ### iRPC
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/iRPC)
    ###
    ### Features
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/Features)
    ###
  ##
#-------------------------------------------------------------------------------

# PROJECT SRC ------------------------------------------------------------------
file(GLOB MAIN_SRC "*.cpp")
  ## dq 
  file(GLOB DQ_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
    ### inData
    file(GLOB RO_PETI_EVENT_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/RoPetiEvent/*.cxx")
    ### inData
    file(GLOB IN_OUT_DATA_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/InOutData/*.cpp")
    ###
    ### DqROOT
    file(GLOB DQ_ROOT_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/MyROOT/*.cpp")
    ###
    ### Wee2UI (Petiroc2UI)
    file(GLOB WEE2UI_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/Wee2UI/*.cpp")
    ###
    ### LyPeti (Lyon Petiroc chip)
    file(GLOB LYPETI_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/LyPeti/*.cpp")
    ###
    ### iRPC
    file(GLOB IRPC_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/iRPC/*.cpp")
    ###
    ### Features
    file(GLOB FEATURES_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/Features/*.cpp")
    ###
  ##
  set(CPPS ${MAIN_SRC} ${RO_PETI_EVENT_SRC} ${DQ_SRC} ${IN_OUT_DATA_SRC} ${DQ_ROOT_SRC} 
    ${WEE2UI_SRC} ${LYPETI_SRC} ${IRPC_SRC} ${FEATURES_SRC})


#-------------------------------------------------------------------------------

# COMPILER SETTING -------------------------------------------------------------

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
if(COMPILER_SUPPORTS_CXX17)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
else()
  CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)
  if(COMPILER_SUPPORTS_CXX14)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
  else()
    if(COMPILER_SUPPORTS_CXX11)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    else()
      message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. 
                      Please update your C++ compiler.")
    endif()
  endif()
endif()
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_EXTENSIONS OFF)
#set(CMAKE_PLATFORM_INDEPENDENT_CODE ON)

add_definitions(-Wall -O3)

#Link to ROOT.

add_executable(${PROJECT_NAME} ${CPPS})
target_link_libraries(${PROJECT_NAME} RoPetiEvent ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})
# LIBRARIES --------------------------------------------------------------------
file(GLOB PCM_FILES "${PROJECT_BINARY_DIR}/*.pcm")
file(COPY ${PCM_FILES} DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
#-------------------------------------------------------------------------------
################################################################################
