#[[
Copyright (c) 2021 Adam Kaniewski

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

find_package(JNI)
if (JNI_FOUND)
  message (STATUS "JNI_INCLUDE_DIRS=${JNI_INCLUDE_DIRS}")
  message (STATUS "JNI_LIBRARIES=${JNI_LIBRARIES}")
else()
  message (FATAL_ERROR "JNI packe not found")
endif()

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

add_definitions(-D__ANDROID__)

#Set patch to ndk here
#set(NDK "PATH TO NDK DIR")
if(NOT DEFINED NDK)
  message(FATAL_ERROR "Edit CMakeLists.txt and set patch to ndk in line : 50")
endif()

set(AINCL "${NDK}toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/include")
set(ALIBS_DIR "${NDK}toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/lib")
set(DEFAULT_CXX "armv7a-linux-androideabi24-clang++")
set(CMAKE_CXX_COMPILER "${NDK}/toolchains/llvm/prebuilt/linux-x86_64/bin/${DEFAULT_CXX}")

set(SRC_DIR "${PROJECT_SOURCE_DIR}/jni")
set(COMMON_DIR "${PROJECT_SOURCE_DIR}/../../common")
set(SACS_DIR "${PROJECT_SOURCE_DIR}/../../")
set(WRAPPERS_DIR "${PROJECT_SOURCE_DIR}/..")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)

add_custom_command(
  OUTPUT  ${SACS_DIR}/WebAppData.cpp
  COMMAND ${SACS_DIR}/web_pack.sh
)

include_directories(
  "${AINCL}"
  "${SRC_DIR}"
  "${SACS_DIR}"
  "${WRAPPERS_DIR}"
  "${SACS_DIR}/json/include"
  "${COMMON_DIR}/third_party/spdlog/include"
  "${COMMON_DIR}/tools/thread"
  "${COMMON_DIR}/tools/logger"
  "${COMMON_DIR}/tools/net"
  "${COMMON_DIR}/tools/net/http"
)

set(COMMON
  ${COMMON_DIR}/tools/thread/PosixThread.cpp
  ${COMMON_DIR}/tools/thread/ThreadLoop.cpp
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
  ${COMMON_DIR}/tools/net/http/HttpServer.cpp
  ${COMMON_DIR}/tools/net/http/MessageBuilderHttp.cpp
)

set(SACS
  ${SACS_DIR}/Module.cpp
  ${SACS_DIR}/JsonMsg.cpp
  ${SACS_DIR}/ModuleManager.cpp
  ${SACS_DIR}/Proxy.cpp
  ${SACS_DIR}/ProxyServer.cpp
  ${SACS_DIR}/HttpServerImpl.cpp
  ${SACS_DIR}/WebAppData.cpp
)

set(JNI
  ${COMMON}
  ${SACS}
  ${SRC_DIR}/MsgThread.cpp
  ${SRC_DIR}/JniGlue.cpp
  ${SRC_DIR}/JniLib.cpp
)

set(LIBS -L${ALIBS_DIR} -static-libstdc++ -llog dl ${CXX_FLAGS})

add_library(sacs_jni SHARED ${JNI})
target_link_libraries(sacs_jni ${LIBS})
