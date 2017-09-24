#include <string>
#include <set>
#include <iostream>
#include <stdexcept>
#include <memory>
#include <chrono>
#include <sstream>

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include <thrift/transport/TBufferTransports.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TCompactProtocol.h>

#include "thrift/gen-cpp/test_types.h"
#include "thrift/gen-cpp/test_constants.h"

#include <capnp/message.h>
#include <capnp/serialize.h>

#include "protobuf/test.pb.h"
#include "capnproto/test.capnp.h"
#include "boost/record.hpp"
#include "msgpack/record.hpp"
#include "cereal/record.hpp"
#include "bitsery/record.hpp"
#include "avro/record.hpp"
#include "flatbuffers/test_generated.h"
#include "yas/record.hpp"

#include "data.hpp"

enum class ThriftSerializationProto { Binary, Compact };

void
thrift_serialization_test(size_t iterations, ThriftSerializationProto proto = ThriftSerializationProto::Binary)
{
    using apache::thrift::transport::TMemoryBuffer;
    using apache::thrift::protocol::TBinaryProtocol;
    using apache::thrift::protocol::TCompactProtocol;
    using apache::thrift::protocol::TBinaryProtocolT;
    using apache::thrift::protocol::TCompactProtocolT;

    using namespace thrift_test;

    boost::shared_ptr<TMemoryBuffer> buffer1(new TMemoryBuffer());
    boost::shared_ptr<TMemoryBuffer> buffer2(new TMemoryBuffer());

    TBinaryProtocolT<TMemoryBuffer> binary_protocol1(buffer1);
    TBinaryProtocolT<TMemoryBuffer> binary_protocol2(buffer2);

    TCompactProtocolT<TMemoryBuffer> compact_protocol1(buffer1);
    TCompactProtocolT<TMemoryBuffer> compact_protocol2(buffer2);

    Record r1;

    for (size_t i = 0; i < kIntegers.size(); i++) {
        r1.ids.push_back(kIntegers[i]);
    }

    for (size_t i = 0; i < kStringsCount; i++) {
        r1.strings.push_back(kStringValue);
    }

    std::string serialized;

    if (proto == ThriftSerializationProto::Binary) {
        r1.write(&binary_protocol1);
    } else if (proto == ThriftSerializationProto::Compact) {
        r1.write(&compact_protocol1);
    }

    serialized = buffer1->getBufferAsString();

    // check if we can deserialize back
    Record r2;

    buffer2->resetBuffer((uint8_t*)serialized.data(), serialized.length());

    if (proto == ThriftSerializationProto::Binary) {
        r2.read(&binary_protocol2);
    } else if (proto == ThriftSerializationProto::Compact) {
        r2.read(&compact_protocol2);
    }

    if (r1 != r2) {
        throw std::logic_error("thrift's case: deserialization failed");
    }

    std::string tag;

    if (proto == ThriftSerializationProto::Binary) {
        tag = "thrift-binary:";
    } else if (proto == ThriftSerializationProto::Compact) {
        tag = "thrift-compact:";
    }

    std::cout << tag << " version = " << VERSION << std::endl;
    std::cout << tag << " size = " << serialized.size() << " bytes" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; i++) {
        buffer1->resetBuffer();

        if (proto == ThriftSerializationProto::Binary) {
            r1.write(&binary_protocol1);
        } else if (proto == ThriftSerializationProto::Compact) {
            r1.write(&compact_protocol1);
        }

        serialized = buffer1->getBufferAsString();
        buffer2->resetBuffer((uint8_t*)serialized.data(), serialized.length());

        if (proto == ThriftSerializationProto::Binary) {
            r2.read(&binary_protocol2);
        } else if (proto == ThriftSerializationProto::Compact) {
            r2.read(&compact_protocol2);
        }
    }
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

    std::cout << tag << " time = " << duration << " milliseconds" << std::endl << std::endl;
}

void
protobuf_serialization_test(size_t iterations)
{
    using namespace protobuf_test;

    Record r1;

    for (size_t i = 0; i < kIntegers.size(); i++) {
        r1.add_ids(kIntegers[i]);
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

    std::cout << "protobuf: version = " << GOOGLE_PROTOBUF_VERSION << std::endl;
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
capnproto_serialization_test(size_t iterations)
{
    using namespace capnp_test;

    capnp::MallocMessageBuilder message;
    Record::Builder r1 = message.getRoot<Record>();

    auto ids = r1.initIds(kIntegers.size());
    for (size_t i = 0; i < kIntegers.size(); i++) {
        ids.set(i, kIntegers[i]);
    }

    auto strings = r1.initStrings(kStringsCount);
    for (size_t i = 0; i < kStringsCount; i++) {
        strings.set(i, kStringValue);
    }

    kj::ArrayPtr<const kj::ArrayPtr<const capnp::word>> serialized = message.getSegmentsForOutput();

    // check if we can deserialize back
    capnp::SegmentArrayMessageReader reader(serialized);
    Record::Reader r2 = reader.getRoot<Record>();
    if (r2.getIds().size() != kIntegers.size()) {
        throw std::logic_error("capnproto's case: deserialization failed");
    }

    size_t size = 0;
    for (auto segment : serialized) {
        size += segment.asBytes().size();
    }

    std::cout << "capnproto: version = " << CAPNP_VERSION << std::endl;
    std::cout << "capnproto: size = " << size << " bytes" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; i++) {
        capnp::MallocMessageBuilder message;
        Record::Builder r1 = message.getRoot<Record>();

        auto ids = r1.initIds(kIntegers.size());
        for (size_t i = 0; i < kIntegers.size(); i++) {
            ids.set(i, kIntegers[i]);
        }

        auto strings = r1.initStrings(kStringsCount);
        for (size_t i = 0; i < kStringsCount; i++) {
            strings.set(i, kStringValue);
        }

        serialized = message.getSegmentsForOutput();
        capnp::SegmentArrayMessageReader reader(serialized);
        auto r2 = reader.getRoot<Record>();

        (void)r2.getIds().size();
        (void)r2.getStrings().size();
    }
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

    std::cout << "capnproto: time = " << duration << " milliseconds" << std::endl << std::endl;
}

void
boost_serialization_test(size_t iterations)
{
    using namespace boost_test;

    Record r1, r2;

    for (size_t i = 0; i < kIntegers.size(); i++) {
        r1.ids.push_back(kIntegers[i]);
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

    std::cout << "boost: version = " << BOOST_VERSION << std::endl;
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

    for (size_t i = 0; i < kIntegers.size(); i++) {
        r1.ids.push_back(kIntegers[i]);
    }

    for (size_t i = 0; i < kStringsCount; i++) {
        r1.strings.push_back(kStringValue);
    }

    msgpack::sbuffer sbuf;

    msgpack::pack(sbuf, r1);

    std::string serialized(sbuf.data(), sbuf.size());

    msgpack::object_handle msg = msgpack::unpack(serialized.data(), serialized.size());

    msgpack::object obj = msg.get();

    obj.convert(r2);

    if (r1 != r2) {
        throw std::logic_error("msgpack's case: deserialization failed");
    }

    std::cout << "msgpack: version = " << msgpack_version() << std::endl;
    std::cout << "msgpack: size = " << serialized.size() << " bytes" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; i++) {
        sbuf.clear();
        msgpack::pack(sbuf, r1);
        msgpack::object_handle msg = msgpack::unpack(sbuf.data(), sbuf.size());
        msgpack::object obj = msg.get();
        obj.convert(r2);
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

    for (size_t i = 0; i < kIntegers.size(); i++) {
        r1.ids.push_back(kIntegers[i]);
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

void
bitsery_serialization_test(size_t iterations)
{
    using namespace bitsery_test;

    Record r1, r2;

    for (size_t i = 0; i < kIntegers.size(); i++) {
        r1.ids.push_back(kIntegers[i]);
    }

    for (size_t i = 0; i < kStringsCount; i++) {
        r1.strings.push_back(kStringValue);
    }

    std::vector<uint8_t> buf;
    r2.strings = r1.strings;
    r2.ids = r1.ids;
    bitsery_serialize(r1, buf);
    bitsery_deserialize(r2, buf);

    if (r1 != r2) {
        throw std::logic_error("bitsery's case: deserialization failed");
    }

    std::cout << "bitsery: size = " << buf.size() << " bytes" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; i++) {
        buf.clear();
        bitsery_serialize(r1, buf);
        bitsery_deserialize(r2, buf);
    }
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

    std::cout << "bitsery: time = " << duration << " milliseconds" << std::endl << std::endl;
}








void
avro_serialization_test(size_t iterations)
{
    using namespace avro_test;

    Record r1, r2;

    for (size_t i = 0; i < kIntegers.size(); i++) {
        r1.ids.push_back(kIntegers[i]);
    }

    for (size_t i = 0; i < kStringsCount; i++) {
        r1.strings.push_back(kStringValue);
    }
    r2.strings.resize(100);

    std::unique_ptr<avro::OutputStream> out = avro::memoryOutputStream();
    avro::EncoderPtr encoder = avro::binaryEncoder();

    encoder->init(*out);
    avro::encode(*encoder, r1);

    auto serialized_size = out->byteCount();

    std::unique_ptr<avro::InputStream> in = avro::memoryInputStream(*out);
    avro::DecoderPtr decoder = avro::binaryDecoder();

    decoder->init(*in);
    avro::decode(*decoder, r2);

    if (r1.ids != r2.ids || r1.strings != r2.strings || r2.ids.size() != kIntegers.size() || r2.strings.size() != kStringsCount) {
        throw std::logic_error("avro's case: deserialization failed");
    }

    std::cout << "avro: size = " << serialized_size << " bytes" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; i++) {
        auto out = avro::memoryOutputStream();
        auto encoder = avro::binaryEncoder();
        encoder->init(*out);
        avro::encode(*encoder, r1);

        auto in = avro::memoryInputStream(*out);
        auto decoder = avro::binaryDecoder();
        decoder->init(*in);
        avro::decode(*decoder, r2);
    }
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

    std::cout << "avro: time = " << duration << " milliseconds" << std::endl << std::endl;
}

void
flatbuffers_serialization_test(size_t iterations)
{
    using namespace flatbuffers_test;

    std::vector<flatbuffers::Offset<flatbuffers::String>> strings;
    strings.reserve(kStringsCount);

    flatbuffers::FlatBufferBuilder builder;
    for (size_t i = 0; i < kStringsCount; i++) {
        strings.push_back(builder.CreateString(kStringValue));
    }

    auto ids_vec = builder.CreateVector(kIntegers);
    auto strings_vec = builder.CreateVector(strings);
    auto r1 = CreateRecord(builder, ids_vec, strings_vec);

    builder.Finish(r1);

    auto p = reinterpret_cast<char*>(builder.GetBufferPointer());
    auto sz = builder.GetSize();
    std::vector<char> buf(p, p + sz);

    auto r2 = GetRecord(buf.data());
    if (r2->strings()->size() != kStringsCount || r2->ids()->size() != kIntegers.size()) {
        throw std::logic_error("flatbuffer's case: deserialization failed");
    }

    std::cout << "flatbuffers: size = " << builder.GetSize() << " bytes" << std::endl;

    builder.ReleaseBufferPointer();

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; i++) {
        flatbuffers::FlatBufferBuilder builder;
        strings.clear();

        for (size_t i = 0; i < kStringsCount; i++) {
            strings.push_back(builder.CreateString(kStringValue));
        }

        auto ids_vec = builder.CreateVector(kIntegers);
        auto strings_vec = builder.CreateVector(strings);
        auto r1 = CreateRecord(builder, ids_vec, strings_vec);
        builder.Finish(r1);

        auto p = reinterpret_cast<char*>(builder.GetBufferPointer());
        auto sz = builder.GetSize();
        std::vector<char> buf(p, p + sz);
        auto r2 = GetRecord(buf.data());
        (void)r2->ids()[0];

        builder.ReleaseBufferPointer();
    }
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

    std::cout << "flatbuffers: time = " << duration << " milliseconds" << std::endl << std::endl;
}

template <std::size_t opts>
void
yas_serialization_test(size_t iterations)
{
    using namespace yas_test;

    Record r1, r2;

    for (size_t i = 0; i < kIntegers.size(); i++) {
        r1.ids.push_back(kIntegers[i]);
    }

    for (size_t i = 0; i < kStringsCount; i++) {
        r1.strings.push_back(kStringValue);
    }

    std::string serialized;

    to_string<opts>(r1, serialized);
    from_string<opts>(r2, serialized);

    if (r1 != r2) {
        throw std::logic_error("yas' case: deserialization failed");
    }

    if (opts & yas::compacted) {
        std::cout << "yas-compact: version = " << YAS_VERSION_STRING << std::endl;
        std::cout << "yas-compact: size = " << serialized.size() << " bytes" << std::endl;
    } else {
        std::cout << "yas: version = " << YAS_VERSION_STRING << std::endl;
        std::cout << "yas: size = " << serialized.size() << " bytes" << std::endl;
    }

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; i++) {
        yas::mem_ostream os;
        yas::binary_oarchive<yas::mem_ostream, opts> oa(os);
        oa& r1;

        yas::mem_istream is(os.get_intrusive_buffer());
        yas::binary_iarchive<yas::mem_istream, opts> ia(is);
        ia& r2;
    }
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

    if (opts & yas::compacted) {
        std::cout << "yas-compact: time = " << duration << " milliseconds" << std::endl << std::endl;
    } else {
        std::cout << "yas: time = " << duration << " milliseconds" << std::endl << std::endl;
    }
}

int
main(int argc, char** argv)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    if (argc < 2) {
        std::cout << "usage: " << argv[0]
                  << " N [thrift-binary thrift-compact protobuf boost msgpack cereal avro capnproto flatbuffers yas yas-compact]";
        std::cout << std::endl << std::endl;
        std::cout << "arguments: " << std::endl;
        std::cout << " N  -- number of iterations" << std::endl << std::endl;
        return EXIT_SUCCESS;
    }

    size_t iterations;

    try {
        iterations = boost::lexical_cast<size_t>(argv[1]);
    } catch (std::exception& exc) {
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

    /*std::cout << "total size: " << sizeof(kIntegerValue) * kIntegersCount + kStringValue.size() * kStringsCount << std::endl;*/

    try {
        if (names.empty() || names.find("thrift-binary") != names.end()) {
            thrift_serialization_test(iterations, ThriftSerializationProto::Binary);
        }

        if (names.empty() || names.find("thrift-compact") != names.end()) {
            thrift_serialization_test(iterations, ThriftSerializationProto::Compact);
        }

        if (names.empty() || names.find("protobuf") != names.end()) {
            protobuf_serialization_test(iterations);
        }

        if (names.empty() || names.find("capnproto") != names.end()) {
            capnproto_serialization_test(iterations);
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

        if (names.empty() || names.find("bitsery") != names.end()) {
            bitsery_serialization_test(iterations);
        }        

        if (names.empty() || names.find("avro") != names.end()) {
            avro_serialization_test(iterations);
        }

        if (names.empty() || names.find("flatbuffers") != names.end()) {
            flatbuffers_serialization_test(iterations);
        }

        if (names.empty() || names.find("yas") != names.end()) {
            yas_serialization_test<yas::binary | yas::no_header>(iterations);
        }

        if (names.empty() || names.find("yas-compact") != names.end()) {
            yas_serialization_test<yas::binary | yas::no_header | yas::compacted>(iterations);
        }
    } catch (std::exception& exc) {
        std::cerr << "Error: " << exc.what() << std::endl;
        return EXIT_FAILURE;
    }

    google::protobuf::ShutdownProtobufLibrary();

    return EXIT_SUCCESS;
}
