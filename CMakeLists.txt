project(test)
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

if (NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "Doesn't support non 64-bit platforms")
endif()

include(FindThreads)
include(CheckTypeSize)
include(CheckIncludeFile)
include(CheckCXXCompilerFlag)
include(ExternalProject)

CHECK_INCLUDE_FILES("inttypes.h" HAVE_INTTYPES_H)
CHECK_INCLUDE_FILES("netinet/in.h" HAVE_NETINET_IN_H)

if (HAVE_INTTYPES_H)
    add_definitions("-DHAVE_INTTYPES_H")
endif()

if (HAVE_NETINET_IN_H)
    add_definitions("-DHAVE_NETINET_IN_H")
endif()

include_directories(${test_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -W")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -W")

if (CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    set(MAKE_PROGRAM "gmake")
    set(BOOST_ROOT "/usr/local")
else()
    set(MAKE_PROGRAM "make")
endif()

find_package(Boost QUIET)
if (NOT Boost_FOUND)
    set(boost_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external/boost)
    ExternalProject_Add(
        boost
        PREFIX ${boost_PREFIX}
        URL "http://netcologne.dl.sourceforge.net/project/boost/boost/1.54.0/boost_1_54_0.tar.gz"
        URL_MD5 "efbfbff5a85a9330951f243d0a46e4b9"
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND mkdir -p ${boost_PREFIX}/include/ && cp -r ${boost_PREFIX}/src/boost/boost ${boost_PREFIX}/include/
    )
    set(Boost_INCLUDE_DIRS ${boost_PREFIX})
    include_directories(${Boost_INCLUDE_DIRS}/include)
    message(STATUS "Using Boost from ${Boost_INCLUDE_DIRS}/include")
else()
    include_directories(${Boost_INCLUDE_DIRS})
    message(STATUS "Using Boost from ${Boost_INCLUDE_DIRS}")
endif()

set(thrift_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external/thrift)
ExternalProject_Add(
    thrift
    PREFIX ${thrift_PREFIX}
    URL "http://www.eu.apache.org/dist/thrift/0.9.1/thrift-0.9.1.tar.gz"
    URL_MD5 "d2e46148f6e800a9492dbd848c66ab6e"
    CONFIGURE_COMMAND CXX=${CMAKE_CXX_COMPILER} CC=${CMAKE_C_COMPILER} ${thrift_PREFIX}/src/thrift/configure --prefix=${thrift_PREFIX} --with-boost=${Boost_INCLUDE_DIRS} --enable-shared=no --with-libevent=no --with-c_glib=no --with-java=no --with-erlang=no --with-python=no --with-perl=no --with-php=no --with-php_extension=no --with-ruby=no --with-haskell=no --with-go=no --with-d=no
    BUILD_COMMAND ${MAKE_PROGRAM}
    INSTALL_COMMAND ${MAKE_PROGRAM} install
    BUILD_IN_SOURCE 1
)
include_directories(${thrift_PREFIX}/include)
set(THRIFT_LIBRARIES ${thrift_PREFIX}/lib/libthrift.a)
set(THRIFT_GENERATOR ${thrift_PREFIX}/bin/thrift)
if (NOT Boost_FOUND)
    add_dependencies(thrift boost)
endif()

set(msgpack_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external/msgpack)
ExternalProject_Add(
    msgpack
    PREFIX ${msgpack_PREFIX}
    URL "http://heanet.dl.sourceforge.net/project/msgpack/msgpack/cpp/msgpack-0.5.4.tar.gz"
    URL_MD5 "18d96a3178f7cad73c0ca44f6284ae7d"
    CONFIGURE_COMMAND CXX=${CMAKE_CXX_COMPILER} CC=${CMAKE_C_COMPILER} ${msgpack_PREFIX}/src/msgpack/configure --prefix=${msgpack_PREFIX} --enable-shared=no
    BUILD_COMMAND ${MAKE_PROGRAM}
    INSTALL_COMMAND ${MAKE_PROGRAM} install
    BUILD_IN_SOURCE 1
)
include_directories(${msgpack_PREFIX}/include)
set(MSGPACK_LIBRARIES ${msgpack_PREFIX}/lib/libmsgpack.a)

set(protobuf_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external/protobuf)
ExternalProject_Add(
    protobuf
    PREFIX ${protobuf_PREFIX}
    URL "http://protobuf.googlecode.com/files/protobuf-2.5.0.tar.gz"
    URL_MD5 "b751f772bdeb2812a2a8e7202bf1dae8"
    CONFIGURE_COMMAND CXX=${CMAKE_CXX_COMPILER} CC=${CMAKE_C_COMPILER} ${protobuf_PREFIX}/src/protobuf/configure --prefix=${protobuf_PREFIX} --enable-shared=no
    BUILD_COMMAND ${MAKE_PROGRAM}
    INSTALL_COMMAND ${MAKE_PROGRAM} install
    BUILD_IN_SOURCE 1
)
include_directories(${protobuf_PREFIX}/include)
set(PROTOBUF_LIBRARIES ${protobuf_PREFIX}/lib/libprotobuf.a)
set(PROTOBUF_GENERATOR ${protobuf_PREFIX}/bin/protoc)

set(LINKLIBS    ${THRIFT_LIBRARIES}
                ${MSGPACK_LIBRARIES}
                ${PROTOBUF_LIBRARIES}
                ${CMAKE_THREAD_LIBS_INIT}
)

add_executable(test ${test_SOURCE_DIR}/test.cpp)
add_dependencies(test thrift msgpack protobuf)
target_link_libraries(test ${LINKLIBS})
