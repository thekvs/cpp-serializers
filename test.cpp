#include <string>
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

    boost::shared_ptr<TMemoryBuffer>   buffer(new TMemoryBuffer());
    boost::shared_ptr<TBinaryProtocol> protocol(new TBinaryProtocol(buffer));

    Record a;

    for (size_t i = 0; i < kItegersCount; i++) {
        a.ids.push_back(kIntegerValue);
    }

    for (size_t i = 0; i < kStringsCount; i++) {
        a.strings.push_back(kStringValue);
    }

    std::string serialized;

    a.write(protocol.get());
    serialized = buffer->getBufferAsString();

    // check if we can deserialize back
    boost::shared_ptr<TMemoryBuffer>   buffer2(new TMemoryBuffer());
    boost::shared_ptr<TBinaryProtocol> protocol2(new TBinaryProtocol(buffer2));

    buffer2->resetBuffer((uint8_t*)serialized.data(), serialized.length());
    Record a2;
    a2.read(protocol2.get());

    if (a != a2) {
        throw std::logic_error("thrift's case: invariant failed");
    }

    std::cout << "thrift: serialized object size = " << serialized.size() << " bytes" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; i++) {
        buffer->resetBuffer();
        a.write(protocol.get());
        serialized = buffer->getBufferAsString();
    }
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

    std::cout << "thrift: serialization time = " << duration << " milliseconds" << std::endl << std::endl;
}

void
protobuf_serialization_test(size_t iterations)
{
    using namespace protobuf_test;

    Record a;

    for (size_t i = 0; i < kItegersCount; i++) {
        a.add_ids(kIntegerValue);
    }

    for (size_t i = 0; i < kStringsCount; i++) {
        a.add_strings(kStringValue);
    }

    std::string serialized;

    a.SerializeToString(&serialized);

    // check if we can deserialize back
    Record a2;
    bool ok = a2.ParseFromString(serialized);
    if (!ok /*|| a2 != a*/) {
        throw std::logic_error("protobuf's case: invariant failed");
    }

    std::cout << "protobuf: serialized object size = " << serialized.size() << " bytes" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; i++) {
        serialized.clear();
        a.SerializeToString(&serialized);
    }
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

    std::cout << "protobuf: serialization time = " << duration << " milliseconds" << std::endl << std::endl;
}

int
main(int argc, char **argv)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " iterations" << std::endl;
        exit(0);
    }

    auto iterations = boost::lexical_cast<size_t>(argv[1]);

    std::cout << "performing " << iterations << " iterations" << std::endl << std::endl;

    /*std::cout << "total size: " << sizeof(kIntegerValue) * kItegersCount + kStringValue.size() * kStringsCount << std::endl;*/

    thrift_serialization_test(iterations);
    protobuf_serialization_test(iterations);

    return 0;
}
