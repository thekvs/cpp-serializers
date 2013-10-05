#include <string>
#include <set>
#include <iostream>
#include <stdexcept>
#include <memory>
#include <chrono>

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include <thrift/transport/TBufferTransports.h>
#include <thrift/protocol/TBinaryProtocol.h>

#include "thrift/gen-cpp/test_types.h"
#include "thrift/gen-cpp/test_constants.h"

#include "protobuf/test.pb.h"

#include "boost/record.hpp"

#include "msgpack/record.hpp"

#include "cereal/record.hpp"

const size_t      kItegersCount = 1000;
const size_t      kStringsCount = 100;
const int64_t     kIntegerValue = 26354;
const std::string kStringValue  = "shgfkghsdfjhgsfjhfgjhfgjsffghgsfdhgsfdfkdjhfioukjhkfdljgdfkgvjafdhasgdfwurtjkghfsdjkfg";

void
thrift_serialization_test(size_t iterations)
{
    using apache::thrift::transport::TMemoryBuffer;
    using apache::thrift::protocol::TBinaryProtocol;
    
    using namespace thrift_test;

    boost::shared_ptr<TMemoryBuffer>   buffer1(new TMemoryBuffer());
    boost::shared_ptr<TBinaryProtocol> protocol1(new TBinaryProtocol(buffer1));

    Record r1;

    for (size_t i = 0; i < kItegersCount; i++) {
        r1.ids.push_back(kIntegerValue);
    }

    for (size_t i = 0; i < kStringsCount; i++) {
        r1.strings.push_back(kStringValue);
    }

    std::string serialized;

    r1.write(protocol1.get());
    serialized = buffer1->getBufferAsString();

    // check if we can deserialize back
    boost::shared_ptr<TMemoryBuffer>   buffer2(new TMemoryBuffer());
    boost::shared_ptr<TBinaryProtocol> protocol2(new TBinaryProtocol(buffer2));

    Record r2;

    buffer2->resetBuffer((uint8_t*)serialized.data(), serialized.length());
    r2.read(protocol2.get());

    if (r1 != r2) {
        throw std::logic_error("thrift's case: deserialization failed");
    }

    std::cout << "thrift: size = " << serialized.size() << " bytes" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; i++) {
        buffer1->resetBuffer();
        r1.write(protocol1.get());
        serialized = buffer1->getBufferAsString();
        buffer2->resetBuffer((uint8_t*)serialized.data(), serialized.length());
        r2.read(protocol2.get());
    }
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

    std::cout << "thrift: time = " << duration << " milliseconds" << std::endl << std::endl;
}

void
protobuf_serialization_test(size_t iterations)
{
    using namespace protobuf_test;

    Record r1;

    for (size_t i = 0; i < kItegersCount; i++) {
        r1.add_ids(kIntegerValue);
    }

    for (size_t i = 0; i < kStringsCount; i++) {
        r1.add_strings(kStringValue);
    }

    std::string serialized;

    r1.SerializeToString(&serialized);

    // check if we can deserialize back
    Record r2;
    bool ok = r2.ParseFromString(serialized);
    if (!ok /*|| r2 != r1*/) {
        throw std::logic_error("protobuf's case: deserialization failed");
    }

    std::cout << "protobuf: size = " << serialized.size() << " bytes" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; i++) {
        serialized.clear();
        r1.SerializeToString(&serialized);
        r2.ParseFromString(serialized);
    }
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

    std::cout << "protobuf: time = " << duration << " milliseconds" << std::endl << std::endl;
}

void
boost_serialization_test(size_t iterations)
{
    using namespace boost_test;

    Record r1, r2;

    for (size_t i = 0; i < kItegersCount; i++) {
        r1.ids.push_back(kIntegerValue);
    }

    for (size_t i = 0; i < kStringsCount; i++) {
        r1.strings.push_back(kStringValue);
    }

    std::string serialized;

    to_string(r1, serialized);
    from_string(r2, serialized);

    if (r1 != r2) {
        throw std::logic_error("boost's case: deserialization failed");
    }

    std::cout << "boost: size = " << serialized.size() << " bytes" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; i++) {
        serialized.clear();
        to_string(r1, serialized);
        from_string(r2, serialized);
    }
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

    std::cout << "boost: time = " << duration << " milliseconds" << std::endl << std::endl;
}

void
msgpack_serialization_test(size_t iterations)
{
    using namespace msgpack_test;

    Record r1, r2;

    for (size_t i = 0; i < kItegersCount; i++) {
        r1.ids.push_back(kIntegerValue);
    }

    for (size_t i = 0; i < kStringsCount; i++) {
        r1.strings.push_back(kStringValue);
    }

    msgpack::sbuffer sbuf;

    msgpack::pack(sbuf, r1);

    std::string serialized(sbuf.data(), sbuf.size());
 
    msgpack::unpacked msg;
    msgpack::unpack(&msg, serialized.data(), serialized.size());
 
    msgpack::object obj = msg.get();
 
    obj.convert(&r2);

    if (r1 != r2) {
        throw std::logic_error("msgpack's case: deserialization failed");   
    }

    std::cout << "msgpack: size = " << serialized.size() << " bytes" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; i++) {
        sbuf.clear();
        msgpack::pack(sbuf, r1);
        msgpack::unpacked msg;
        msgpack::unpack(&msg, sbuf.data(), sbuf.size());
        msgpack::object obj = msg.get();
        obj.convert(&r2);                
    }
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

    std::cout << "msgpack: time = " << duration << " milliseconds" << std::endl << std::endl;
}

void
cereal_serialization_test(size_t iterations)
{
    using namespace cereal_test;

    Record r1, r2;

    for (size_t i = 0; i < kItegersCount; i++) {
        r1.ids.push_back(kIntegerValue);
    }

    for (size_t i = 0; i < kStringsCount; i++) {
        r1.strings.push_back(kStringValue);
    }

    std::string serialized;

    to_string(r1, serialized);
    from_string(r2, serialized);

    if (r1 != r2) {
        throw std::logic_error("cereal's case: deserialization failed");
    }

    std::cout << "cereal: size = " << serialized.size() << " bytes" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; i++) {
        serialized.clear();
        to_string(r1, serialized);
        from_string(r2, serialized);
    }
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

    std::cout << "cereal: time = " << duration << " milliseconds" << std::endl << std::endl;
}

int
main(int argc, char **argv)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    if (argc < 2) {
        std::cout << "usage: " << argv[0] << " N [thrift protobuf boost msgpack cereal]" << std::endl << std::endl;
        std::cout << "arguments: " << std::endl;
        std::cout << " N  -- number of iterations" << std::endl << std::endl;
        return EXIT_SUCCESS;
    }

    size_t iterations;

    try {
        iterations = boost::lexical_cast<size_t>(argv[1]);
    } catch (std::exception &exc) {
        std::cerr << "Error: " << exc.what() << std::endl;
        std::cerr << "First positional argument must be an integer." << std::endl;
        return EXIT_FAILURE;
    }

    std::set<std::string> names;

    if (argc > 2) {
        for (int i = 2; i < argc; i++) {
            names.insert(argv[i]);
        }
    }

    std::cout << "performing " << iterations << " iterations" << std::endl << std::endl;

    /*std::cout << "total size: " << sizeof(kIntegerValue) * kItegersCount + kStringValue.size() * kStringsCount << std::endl;*/

    try {
        if (names.empty() || names.find("thrift") != names.end()) {
            thrift_serialization_test(iterations);
        }

        if (names.empty() || names.find("protobuf") != names.end()) {
            protobuf_serialization_test(iterations);
        }

        if (names.empty() || names.find("boost") != names.end()) {
            boost_serialization_test(iterations);
        }

        if (names.empty() || names.find("msgpack") != names.end()) {
            msgpack_serialization_test(iterations);
        }

        if (names.empty() || names.find("cereal") != names.end()) {
            cereal_serialization_test(iterations);
        }
    } catch (std::exception &exc) {
        std::cerr << "Error: " << exc.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
