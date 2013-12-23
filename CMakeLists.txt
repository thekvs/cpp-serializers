project(cpp_serializers)
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

if (NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "Doesn't support non 64-bit platforms")
endif()

include(FindThreads)
include(CheckTypeSize)
include(CheckIncludeFile)
include(CheckCXXCompilerFlag)
include(ExternalProject)

CHECK_CXX_COMPILER_FLAG("-std=c++11" CXX11)
if (NOT CXX11)
    message(FATAL_ERROR "C++ compiler doesn't support C++11")
endif()

CHECK_INCLUDE_FILES("inttypes.h" HAVE_INTTYPES_H)
CHECK_INCLUDE_FILES("netinet/in.h" HAVE_NETINET_IN_H)

if (HAVE_INTTYPES_H)
    add_definitions("-DHAVE_INTTYPES_H")
endif()

if (HAVE_NETINET_IN_H)
    add_definitions("-DHAVE_NETINET_IN_H")
endif()

include_directories(${cpp_serializers_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -W")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -W")

if (CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    set(MAKE_PROGRAM "gmake")
    set(BOOST_ROOT "/usr/local")
else()
    set(MAKE_PROGRAM "make")
endif()

set(boost_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external/boost)
ExternalProject_Add(
    boost
    PREFIX ${boost_PREFIX}
    URL "http://netcologne.dl.sourceforge.net/project/boost/boost/1.55.0/boost_1_55_0.tar.gz"
    URL_MD5 "93780777cfbf999a600f62883bd54b17"
    CONFIGURE_COMMAND ${boost_PREFIX}/src/boost/bootstrap.sh --with-libraries=serialization --prefix=${boost_PREFIX}
    BUILD_COMMAND ./bjam link=static
    INSTALL_COMMAND ./bjam link=static install
    BUILD_IN_SOURCE 1
)
set(Boost_INCLUDE_DIRS ${boost_PREFIX})
set(BOOST_LIBRARIES ${boost_PREFIX}/lib/libboost_serialization.a)
include_directories(${Boost_INCLUDE_DIRS}/include)
message(STATUS "Using Boost from ${Boost_INCLUDE_DIRS}/include")

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
    URL "https://github.com/msgpack/msgpack-c/releases/download/cpp-0.5.8/msgpack-0.5.8.tar.gz"
    URL_MD5 "ea0bee0939d2980c0df91f0e4843ccc4"
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

set(cereal_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external/cereal)
ExternalProject_Add(
    cereal
    PREFIX ${cereal_PREFIX}
    URL "https://github.com/USCiLab/cereal/archive/v0.9.1.tar.gz"
    URL_MD5 "8872d4444ff274ce6cd1ed364d0fc0ad"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND mkdir -p ${cereal_PREFIX}/include/ && cp -r ${cereal_PREFIX}/src/cereal/include/cereal ${cereal_PREFIX}/include/
)
include_directories(${cereal_PREFIX}/include)

set(LINKLIBS    ${THRIFT_LIBRARIES}
                ${MSGPACK_LIBRARIES}
                ${PROTOBUF_LIBRARIES}
                ${BOOST_LIBRARIES}
                ${CMAKE_THREAD_LIBS_INIT}
)

add_custom_command(
    DEPENDS ${cpp_serializers_SOURCE_DIR}/test.thrift
    COMMAND ${THRIFT_GENERATOR}
    ARGS -r -gen cpp -o ${cpp_serializers_SOURCE_DIR}/thrift/ ${cpp_serializers_SOURCE_DIR}/test.thrift
    OUTPUT  "${cpp_serializers_SOURCE_DIR}/thrift/gen-cpp/test_constants.cpp"
            "${cpp_serializers_SOURCE_DIR}/thrift/gen-cpp/test_types.cpp"
    COMMENT "Executing Thrift compiler"
)
set_source_files_properties(
    ${cpp_serializers_SOURCE_DIR}/thrift/gen-cpp/test_constants.cpp
    ${cpp_serializers_SOURCE_DIR}/thrift/gen-cpp/test_types.cpp
    ${cpp_serializers_SOURCE_DIR}/thrift/gen-cpp/test_constants.h
    ${cpp_serializers_SOURCE_DIR}/thrift/gen-cpp/test_types.h
    PROPERTIES GENERATED TRUE
)
set(THRIFT_SERIALIZATION_SOURCES    ${cpp_serializers_SOURCE_DIR}/thrift/gen-cpp/test_constants.cpp
                                    ${cpp_serializers_SOURCE_DIR}/thrift/gen-cpp/test_types.cpp
)

add_custom_command(
    DEPENDS ${cpp_serializers_SOURCE_DIR}/test.proto
    COMMAND ${PROTOBUF_GENERATOR}
    ARGS -I${cpp_serializers_SOURCE_DIR} --cpp_out=${cpp_serializers_SOURCE_DIR}/protobuf ${cpp_serializers_SOURCE_DIR}/test.proto
    OUTPUT  "${cpp_serializers_SOURCE_DIR}/protobuf/test.pb.cc"
    COMMENT "Executing Protobuf compiler"
)
set_source_files_properties(
    ${cpp_serializers_SOURCE_DIR}/protobuf/test.pb.cc
    ${cpp_serializers_SOURCE_DIR}/protobuf/test.pb.h
    PROPERTIES GENERATED TRUE
)
set(PROTOBUF_SERIALIZATION_SOURCES  ${cpp_serializers_SOURCE_DIR}/protobuf/test.pb.cc)

set(BOOST_SERIALIZATION_SOURCES ${cpp_serializers_SOURCE_DIR}/boost/record.cpp)

set(CEREAL_SERIALIZATION_SOURCES ${cpp_serializers_SOURCE_DIR}/cereal/record.cpp)

add_executable(test ${cpp_serializers_SOURCE_DIR}/test.cpp
                    ${THRIFT_SERIALIZATION_SOURCES}
                    ${PROTOBUF_SERIALIZATION_SOURCES}
                    ${BOOST_SERIALIZATION_SOURCES}
                    ${CEREAL_SERIALIZATION_SOURCES}
)
add_dependencies(test thrift msgpack protobuf boost cereal)
target_link_libraries(test ${LINKLIBS})
set_target_properties(test PROPERTIES COMPILE_FLAGS "-std=c++11")
