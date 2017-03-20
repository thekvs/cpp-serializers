project(cpp_serializers)
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

if (NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "Doesn't support non 64-bit platforms")
endif()

include(FindThreads)
include(CheckTypeSize)
include(CheckIncludeFiles)
include(CheckCXXCompilerFlag)
include(ExternalProject)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

find_package(ZLIB)
if (NOT ZLIB_FOUND)
    message(FATAL_ERROR "Couldn't find zlib library")
endif()
include_directories(${ZLIB_INCLUDE_DIRS})

find_program(AUTORECONF NAMES autoreconf)
if (NOT AUTORECONF)
    message(FATAL_ERROR "Autoreconf not found")
else()
    message(STATUS "Found autoreconf: ${AUTORECONF}")
endif()

find_program(LIBTOOL NAMES libtool libtoolize)
if (NOT LIBTOOL)
    message(FATAL_ERROR "Libtool not found")
else()
    message(STATUS "Found libtool: ${LIBTOOL}")
endif()

set(boost_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external/boost)
ExternalProject_Add(
    boost
    PREFIX ${boost_PREFIX}
    URL "http://netcologne.dl.sourceforge.net/project/boost/boost/1.62.0/boost_1_62_0.tar.gz"
    URL_MD5 "6f4571e7c5a66ccc3323da6c24be8f05"
    CONFIGURE_COMMAND ${boost_PREFIX}/src/boost/bootstrap.sh --with-libraries=serialization,filesystem,system,program_options,iostreams --prefix=${boost_PREFIX}
    BUILD_COMMAND ./bjam link=static cxxflags='-fPIC'
    INSTALL_COMMAND ./bjam link=static install
    BUILD_IN_SOURCE 1
    LOG_UPDATE ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
    LOG_INSTALL ON
)
set(Boost_INCLUDE_DIRS ${boost_PREFIX})
set(BOOST_LIBRARIES ${boost_PREFIX}/lib/libboost_serialization.a)
include_directories(${Boost_INCLUDE_DIRS}/include)
message(STATUS "Using Boost from ${Boost_INCLUDE_DIRS}/include")

set(thrift_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external/thrift)
ExternalProject_Add(
    thrift
    PREFIX ${thrift_PREFIX}
    DEPENDS boost
    URL "http://www.eu.apache.org/dist/thrift/0.9.3/thrift-0.9.3.tar.gz"
    URL_MD5 "88d667a8ae870d5adeca8cb7d6795442"
    CONFIGURE_COMMAND CXX=${CMAKE_CXX_COMPILER} CC=${CMAKE_C_COMPILER} ${thrift_PREFIX}/src/thrift/configure --prefix=${thrift_PREFIX} --libdir=${thrift_PREFIX}/lib --with-boost=${Boost_INCLUDE_DIRS} --enable-shared=no --with-libevent=no --with-c_glib=no --with-java=no --with-erlang=no --with-python=no --with-perl=no --with-php=no --with-php_extension=no --with-ruby=no --with-haskell=no --with-go=no --with-d=no --with-lua=no --with-qt4=no --with-nodejs=no --disable-tests
    BUILD_COMMAND $(MAKE)
    INSTALL_COMMAND $(MAKE) install
    BUILD_IN_SOURCE 1
    LOG_UPDATE ON
    LOG_BUILD ON
    LOG_INSTALL ON
)
include_directories(${thrift_PREFIX}/include)
set(THRIFT_LIBRARIES ${thrift_PREFIX}/lib/libthrift.a)
set(THRIFT_GENERATOR ${thrift_PREFIX}/bin/thrift)

set(msgpack_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external/msgpack)
ExternalProject_Add(
    msgpack
    PREFIX ${msgpack_PREFIX}
    URL "https://github.com/msgpack/msgpack-c/releases/download/cpp-2.0.0/msgpack-2.0.0.tar.gz"
    URL_MD5 "55148cd856c72f954a6eb9cc889a7d2a"
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${msgpack_PREFIX} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    BUILD_IN_SOURCE 1
    LOG_UPDATE ON
    LOG_BUILD ON
    LOG_INSTALL ON
)
include_directories(${msgpack_PREFIX}/include)
set(MSGPACK_LIBRARIES ${msgpack_PREFIX}/lib/libmsgpackc.a)

set(protobuf_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external/protobuf)
ExternalProject_Add(
    protobuf
    PREFIX ${protobuf_PREFIX}
    URL "https://github.com/google/protobuf/releases/download/v3.1.0/protobuf-cpp-3.1.0.tar.gz"
    URL_MD5 "bd5e3eed635a8d32e2b99658633815ef"
    CONFIGURE_COMMAND CXX=${CMAKE_CXX_COMPILER} CC=${CMAKE_C_COMPILER} ${protobuf_PREFIX}/src/protobuf/configure --prefix=${protobuf_PREFIX} --libdir=${protobuf_PREFIX}/lib --enable-shared=no
    BUILD_COMMAND $(MAKE)
    INSTALL_COMMAND $(MAKE) install
    BUILD_IN_SOURCE 1
    LOG_UPDATE ON
    LOG_BUILD ON
    LOG_INSTALL ON
)
include_directories(${protobuf_PREFIX}/include)
set(PROTOBUF_LIBRARIES ${protobuf_PREFIX}/lib/libprotobuf.a)
set(PROTOBUF_GENERATOR ${protobuf_PREFIX}/bin/protoc)

set(capnproto_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external/capnproto)
ExternalProject_Add(
    capnproto
    PREFIX ${capnproto_PREFIX}
    URL "https://github.com/sandstorm-io/capnproto/archive/v0.5.3.tar.gz"
    URL_MD5 "909bd13ad6b8bc840ac78ab8f5bcb0a4"
    CONFIGURE_COMMAND CXX=${CMAKE_CXX_COMPILER} CC=${CMAKE_C_COMPILER} cd ${capnproto_PREFIX}/src/capnproto/c++ && ${AUTORECONF} -i && cd - && ${capnproto_PREFIX}/src/capnproto/c++/configure --prefix=${capnproto_PREFIX} --libdir=${capnproto_PREFIX}/lib --enable-shared=no
    BUILD_COMMAND $(MAKE)
    INSTALL_COMMAND $(MAKE) install
    BUILD_IN_SOURCE 1
    LOG_UPDATE ON
    LOG_BUILD ON
    LOG_INSTALL ON
)
include_directories(${capnproto_PREFIX}/include)
set(CAPNPROTO_LIBRARIES ${capnproto_PREFIX}/lib/libcapnp.a ${capnproto_PREFIX}/lib/libkj.a)
set(CAPNPROTO_GENERATOR ${capnproto_PREFIX}/bin/capnp)
set(CAPNPROTO_CPP_GENERATOR ${capnproto_PREFIX}/bin/capnpc-c++)

set(cereal_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external/cereal)
ExternalProject_Add(
    cereal
    PREFIX ${cereal_PREFIX}
    URL "https://github.com/USCiLab/cereal/archive/v1.2.1.tar.gz"
    URL_MD5 "64476ed74c19068ee543b53ad3992261"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND mkdir -p ${cereal_PREFIX}/include/ && cp -r ${cereal_PREFIX}/src/cereal/include/cereal ${cereal_PREFIX}/include/
)
include_directories(${cereal_PREFIX}/include)

set(avro_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external/avro)
ExternalProject_Add(
    avro
    DEPENDS boost
    PREFIX ${avro_PREFIX}
    URL "http://www.eu.apache.org/dist/avro/avro-1.8.1/avro-src-1.8.1.tar.gz"
    URL_MD5 "bc1901bd2a83956dbb9e7f968affc607"
    CONFIGURE_COMMAND cmake -DBOOST_INCLUDEDIR=${boost_PREFIX}/include -DBOOST_LIBRARYDIR=${boost_PREFIX}/lib -DBoost_NO_SYSTEM_PATHS=ON -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_INSTALL_PREFIX=${avro_PREFIX} -DBUILD_SHARED_LIBS=OFF ${avro_PREFIX}/src/avro/lang/c++/
    # Due to some issues with using statically built boost libraries AND zlib in avro tests
    # build only necessary parts and install them in a very hackish way.
    BUILD_COMMAND $(MAKE) avrocpp_s avrogencpp
    INSTALL_COMMAND mkdir -p ${avro_PREFIX}/include/avro ${avro_PREFIX}/lib ${avro_PREFIX}/bin && cp ${avro_PREFIX}/src/avro-build/avrogencpp ${avro_PREFIX}/bin && cp ${avro_PREFIX}/src/avro-build/libavrocpp_s.a ${avro_PREFIX}/lib && cd ${avro_PREFIX}/src/avro/lang/c++/api/ && cp -r . ${avro_PREFIX}/include/avro/
    LOG_UPDATE ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
)
include_directories(${avro_PREFIX}/include)
set(AVRO_LIBRARIES ${avro_PREFIX}/lib/libavrocpp_s.a)
set(AVRO_GENERATOR ${avro_PREFIX}/bin/avrogencpp)

set(flatbuffers_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external/flatbuffers)
ExternalProject_Add(
    flatbuffers
    PREFIX ${flatbuffers_PREFIX}
    URL "https://github.com/google/flatbuffers/archive/v1.4.0.tar.gz"
    URL_MD5 "03d5b0e6233362b0951f22374d36b553"
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${flatbuffers_PREFIX} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    LOG_UPDATE ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
)
include_directories(${flatbuffers_PREFIX}/include)
set(FLATBUFFERS_LIBRARIES ${flatbuffers_PREFIX}/lib/libflatbuffers.a)
set(FLATBUFFERS_GENERATOR ${flatbuffers_PREFIX}/bin/flatc)

set(yas_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external/yas)
ExternalProject_Add(
    yas
    PREFIX ${yas_PREFIX}
    URL "https://github.com/niXman/yas/archive/3.0.zip"
    URL_MD5 "906ac6654557052c6bfffbe7aa8024a8"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND mkdir -p ${yas_PREFIX}/include/ && cp -r ${yas_PREFIX}/src/yas/include/yas ${yas_PREFIX}/include/
)
include_directories(${yas_PREFIX}/include)

set(LINKLIBS
    ${THRIFT_LIBRARIES}
    ${MSGPACK_LIBRARIES}
    ${PROTOBUF_LIBRARIES}
    ${CAPNPROTO_LIBRARIES}
    ${BOOST_LIBRARIES}
    ${AVRO_LIBRARIES}
    ${FLATBUFFERS_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${ZLIB_LIBRARIES}
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

add_custom_command(
    DEPENDS ${cpp_serializers_SOURCE_DIR}/test.capnp
    COMMAND ${CAPNPROTO_GENERATOR}
    ARGS compile -I${cpp_serializers_SOURCE_DIR} --src-prefix=${cpp_serializers_SOURCE_DIR} -o${CAPNPROTO_CPP_GENERATOR}:${cpp_serializers_SOURCE_DIR}/capnproto ${cpp_serializers_SOURCE_DIR}/test.capnp
    OUTPUT  "${cpp_serializers_SOURCE_DIR}/capnproto/test.capnp.c++"
    COMMENT "Executing Cap'n Proto compiler"
)
set_source_files_properties(
    ${cpp_serializers_SOURCE_DIR}/capnproto/test.capnp.c++
    ${cpp_serializers_SOURCE_DIR}/capnproto/test.capnp.h
    PROPERTIES GENERATED TRUE
)
set(CAPNPROTO_SERIALIZATION_SOURCES  ${cpp_serializers_SOURCE_DIR}/capnproto/test.capnp.c++)

add_custom_command(
    DEPENDS ${cpp_serializers_SOURCE_DIR}/test.json
    COMMAND ${AVRO_GENERATOR}
    ARGS --input ${cpp_serializers_SOURCE_DIR}/test.json --output ${cpp_serializers_SOURCE_DIR}/avro/record.hpp --namespace avro_test
    OUTPUT  "${cpp_serializers_SOURCE_DIR}/avro/record.hpp"
    COMMENT "Executing Avro compiler"
)
set_source_files_properties(
    ${cpp_serializers_SOURCE_DIR}/avro/record.hpp
    PROPERTIES GENERATED TRUE
)
set(AVRO_SERIALIZATION_SOURCES ${cpp_serializers_SOURCE_DIR}/avro/record.hpp)

add_custom_command(
    DEPENDS ${cpp_serializers_SOURCE_DIR}/test.fbs
    COMMAND ${FLATBUFFERS_GENERATOR}
    ARGS --cpp -o ${cpp_serializers_SOURCE_DIR}/flatbuffers ${cpp_serializers_SOURCE_DIR}/test.fbs
    OUTPUT "${cpp_serializers_SOURCE_DIR}/flatbuffers/test_generated.h"
    COMMENT "Executing FlatBuffers compiler"
)
set_source_files_properties(
    ${cpp_serializers_SOURCE_DIR}/flatbuffers/test_generated.h
    PROPERTIES GENERATED TRUE
)
set(FLATBUFFERS_SERIALIZATION_SOURCES ${cpp_serializers_SOURCE_DIR}/flatbuffers/test_generated.h)

set(BOOST_SERIALIZATION_SOURCES ${cpp_serializers_SOURCE_DIR}/boost/record.cpp)
set(CEREAL_SERIALIZATION_SOURCES ${cpp_serializers_SOURCE_DIR}/cereal/record.cpp)
set(YAS_SERIALIZATION_SOURCES ${cpp_serializers_SOURCE_DIR}/yas/record.cpp)

add_executable(
    benchmark
    ${cpp_serializers_SOURCE_DIR}/benchmark.cpp
    ${THRIFT_SERIALIZATION_SOURCES}
    ${PROTOBUF_SERIALIZATION_SOURCES}
    ${CAPNPROTO_SERIALIZATION_SOURCES}
    ${BOOST_SERIALIZATION_SOURCES}
    ${CEREAL_SERIALIZATION_SOURCES}
    ${AVRO_SERIALIZATION_SOURCES}
    ${FLATBUFFERS_SERIALIZATION_SOURCES}
    ${YAS_SERIALIZATION_SOURCES}
)
add_dependencies(benchmark thrift msgpack protobuf capnproto boost cereal avro flatbuffers yas)
target_link_libraries(benchmark ${LINKLIBS})
set_target_properties(benchmark PROPERTIES COMPILE_FLAGS "-std=c++11")
