#[[
Copyright (c) 2020 Adam Kaniewski

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
]]

cmake_minimum_required(VERSION 3.2)
project(Sacs)

set(CXX_FLAGS
 "-O2 \
 -std=gnu++11 \
 -Wall \
 -Werror \
 -Wundef \
 -Wcast-align \
 -Wcast-qual \
 -Wno-unused \
 -Wno-delete-non-virtual-dtor"
)

set(LD_FLAGS
 "-lpthread \
  -ldl"
)

#add_definitions(-DENABLE_DEBUG_LOGGER)

set(CMAKE_CXX_FLAGS "${CXX_FLAGS}")
set(CMAKE_SYSTEM_NAME linux)
set(DEFAULT_CXX "g++")

set(COMMON_DIR "${PROJECT_SOURCE_DIR}/common")
set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(EXAMPLE_DIR "${SRC_DIR}/examples")

if(DEFINED ENV{CXX})
  message("Using user's compiler : " $ENV{CXX})
  set(CMAKE_CXX_COMPILER $ENV{CXX})
else(DEFINED ENV{CXX})
  message("Using default compiler : " ${DEFAULT_CXX})
  set(CMAKE_CXX_COMPILER ${DEFAULT_CXX})
endif(DEFINED ENV{CXX})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build_${CMAKE_CXX_COMPILER})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build_${CMAKE_CXX_COMPILER})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build_${CMAKE_CXX_COMPILER})


include_directories(
  "${SRC_DIR}/wrappers"
  "${SRC_DIR}/json/include"
  "${COMMON_DIR}/third_party/spdlog/include"
  "${COMMON_DIR}/tools/thread"
  "${COMMON_DIR}/tools/logger"
  "${COMMON_DIR}/tools/net")

set(COMMON
  ${COMMON_DIR}/tools/thread/PosixThread.cpp
  ${COMMON_DIR}/tools/net/SocketContext.cpp
  ${COMMON_DIR}/tools/net/SocketObject.cpp
  ${COMMON_DIR}/tools/net/Client.cpp
  ${COMMON_DIR}/tools/net/Connection.cpp
  ${COMMON_DIR}/tools/net/Transporter.cpp
  ${COMMON_DIR}/tools/net/ConnectThread.cpp
  ${COMMON_DIR}/tools/net/Epool.cpp
  ${COMMON_DIR}/tools/net/Message.cpp
  ${COMMON_DIR}/tools/net/MessageBuilder.cpp
  ${COMMON_DIR}/tools/net/Server.cpp
)

set(SACS
  ${COMMON}
  ${SRC_DIR}/Module.cpp
  ${SRC_DIR}/JsonMsg.cpp
  ${SRC_DIR}/ModuleManager.cpp
  ${SRC_DIR}/Proxy.cpp
  ${SRC_DIR}/ProxyServer.cpp
  ${SRC_DIR}/HttpServer.cpp
  ${SRC_DIR}/SacsLib.cpp
)

add_library(sacs SHARED ${SACS})
target_link_libraries(sacs pthread)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build_${CMAKE_CXX_COMPILER}/examples)

add_executable(start ${EXAMPLE_DIR}/Start.cpp)
target_link_libraries(start ${LD_FLAGS})

add_executable(button ${EXAMPLE_DIR}/ButtonAndLog.cpp)
target_link_libraries(button ${LD_FLAGS})

add_executable(dynamic ${EXAMPLE_DIR}/DynamicRegisterAndUnregister.cpp)
target_link_libraries(dynamic ${LD_FLAGS})

add_executable(script ${EXAMPLE_DIR}/Script.cpp)
target_link_libraries(script ${LD_FLAGS})

add_executable(text ${EXAMPLE_DIR}/TextFields.cpp)
target_link_libraries(text ${LD_FLAGS})
