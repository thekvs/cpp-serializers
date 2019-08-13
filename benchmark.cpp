#include <string>
#include <set>
#include <iostream>
#include <stdexcept>
#include <memory>
#include <chrono>
#include <sstream>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include <thrift/transport/TBufferTransports.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TCompactProtocol.h>

#include "thrift/gen-cpp/test_types.h"
#include "thrift/gen-cpp/test_constants.h"

#include <capnp/message.h>
#include <capnp/serialize.h>

#include <cxxopts.hpp>

#include "protobuf/test.pb.h"
#include "capnproto/test.capnp.h"
#include "boost/record.hpp"
#include "msgpack/record.hpp"
#include "cereal/record.hpp"
#include "avro/record.hpp"
#include "flatbuffers/test_generated.h"
#include "yas/record.hpp"

#include "data.hpp"

enum class ThriftSerializationProto { Binary, Compact };

struct Args {
    std::size_t iterations = 0;
    std::set<std::string> serializers;
    bool csv = false;
};

struct Result {
    Result(std::string name, std::string version, size_t size, int64_t time)
        : name(name)
        , version(version)
        , size(size)
        , time(time)
    {
    }

    Result(std::string name, uint64_t version, size_t size, int64_t time)
        : name(name)
        , version(boost::lexical_cast<std::string>(version))
        , size(size)
        , time(time)
    {
    }

    std::string name;
    std::string version;
    size_t size = 0;
    int64_t time = 0;
};

// clang-format off
const std::set<std::string> valid_serializers = {
    "thrift-binary",
    "thrift-compact",
    "protobuf",
    "boost",
    "msgpack",
    "cereal",
    "avro",
    "capnproto",
    "flatbuffers",
    "yas",
    "yas-compact"
};
// clang-format on

void
print_results(Args args, std::vector<Result> results)
{
    if (args.csv) { // print CSV header
        std::cout << "serializer,version,iterations,size,time" << std::endl;
    }

    for (const auto& result : results) {
        if (args.csv) {
            std::cout << result.name << "," << result.version << "," << args.iterations << "," << result.size << ","
                      << result.time << std::endl;
        } else {
            std::cout << "Serializer: " << result.name << std::endl;
            std::cout << "Version   : " << result.version << std::endl;
            std::cout << "Iterations: " << args.iterations << std::endl;
            std::cout << "Size      : " << result.size << " bytes" << std::endl;
            std::cout << "Time      : " << result.time << " milliseconds" << std::endl;
            std::cout << std::endl;
        }
    }
}

void
print_supported_serializers()
{
    std::cerr << std::endl << "Supported serializers are:" << std::endl;
    for (const auto& s : valid_serializers) {
        std::cout << "  * " << s << std::endl;
    }
    std::cout << std::endl;
}

Args
parse_args(int argc, char** argv)
{
    cxxopts::Options args("benchmark", "Benchmark various C++ serializers");

    // clang-format off
    args.add_options()
        ("h,help", "show this help and exit")
        ("l,list", "show list of supported serializers")
        ("c,csv", "output in CSV format")
        ("i,iterations", "number of serialize/deserialize iterations", cxxopts::value<std::size_t>())
        ("s,serializers", "comma separated list of serializers to benchmark", cxxopts::value<std::string>())
        ;
    // clang-format on

    Args opts;

    try {
        auto parsed_opts = args.parse(argc, argv);
        if (parsed_opts.count("help")) {
            std::cout << args.help() << std::endl;
            exit(EXIT_SUCCESS);
        }

        if (parsed_opts.count("list")) {
            print_supported_serializers();
            exit(EXIT_SUCCESS);
        }

        if (parsed_opts.count("iterations") == 0) {
            std::cerr << "Not all required option specified! Please run with -h for available options." << std::endl;
            exit(EXIT_FAILURE);
        }

        if (parsed_opts.count("serializers")) {
            boost::split(opts.serializers, parsed_opts["serializers"].as<std::string>(), boost::is_any_of(","));
            for (const auto& serializer : opts.serializers) {
                auto exists = valid_serializers.find(serializer);
                if (exists == valid_serializers.end()) {
                    std::cerr << "Serializer '" << serializer << "' is not supported by this benchmark." << std::endl;
                    print_supported_serializers();
                    exit(EXIT_FAILURE);
                }
            }
        }

        opts.iterations = parsed_opts["iterations"].as<std::size_t>();

        if (parsed_opts.count("csv")) {
            opts.csv = true;
        }
    } catch (std::exception& exc) {
        std::cerr << exc.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    return opts;
}

Result
thrift_serialization_test(size_t iterations, ThriftSerializationProto proto = ThriftSerializationProto::Binary)
{
    using apache::thrift::protocol::TBinaryProtocol;
    using apache::thrift::protocol::TBinaryProtocolT;
    using apache::thrift::protocol::TCompactProtocol;
    using apache::thrift::protocol::TCompactProtocolT;
    using apache::thrift::transport::TMemoryBuffer;

    using namespace thrift_test;

    std::shared_ptr<TMemoryBuffer> buffer1(new TMemoryBuffer());
    std::shared_ptr<TMemoryBuffer> buffer2(new TMemoryBuffer());

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
        tag = "thrift-binary";
    } else if (proto == ThriftSerializationProto::Compact) {
        tag = "thrift-compact";
    }

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

    return Result(tag, PACKAGE_VERSION, serialized.size(), duration);
}

Result
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

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; i++) {
        serialized.clear();
        r1.SerializeToString(&serialized);
        r2.ParseFromString(serialized);
    }
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

    return Result("protobuf", GOOGLE_PROTOBUF_VERSION, serialized.size(), duration);
}

Result
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

    return Result("capnproto", CAPNP_VERSION, size, duration);
}

Result
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

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; i++) {
        serialized.clear();
        to_string(r1, serialized);
        from_string(r2, serialized);
    }
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

    return Result("boost", BOOST_VERSION, serialized.size(), duration);
}

Result
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

    return Result("msgpack", msgpack_version(), serialized.size(), duration);
}

Result
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

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; i++) {
        serialized.clear();
        to_string(r1, serialized);
        from_string(r2, serialized);
    }
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

    return Result("cereal", "", serialized.size(), duration);
}

Result
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

    std::unique_ptr<avro::OutputStream> out = avro::memoryOutputStream();
    avro::EncoderPtr encoder = avro::binaryEncoder();

    encoder->init(*out);
    avro::encode(*encoder, r1);

    auto serialized_size = out->byteCount();

    std::unique_ptr<avro::InputStream> in = avro::memoryInputStream(*out);
    avro::DecoderPtr decoder = avro::binaryDecoder();

    decoder->init(*in);
    avro::decode(*decoder, r2);

    if (r1.ids != r2.ids || r1.strings != r2.strings || r2.ids.size() != kIntegers.size()
        || r2.strings.size() != kStringsCount) {
        throw std::logic_error("avro's case: deserialization failed");
    }

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

    return Result("avro", "", serialized_size, duration);
}

Result
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

    auto size = builder.GetSize();

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

    auto version = FLATBUFFERS_STRING(FLATBUFFERS_VERSION_MAJOR) "." FLATBUFFERS_STRING(
        FLATBUFFERS_VERSION_MINOR) "." FLATBUFFERS_STRING(FLATBUFFERS_VERSION_REVISION);

    return Result("flatbuffers", version, size, duration);
}

template <std::size_t opts>
Result
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

    std::string tag;

    if (opts & yas::compacted) {
        tag = "yas-compact";
    } else {
        tag = "yas";
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

    return Result(tag, YAS_VERSION_STRING, serialized.size(), duration);
}

int
main(int argc, char** argv)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    auto args = parse_args(argc, argv);

    /*std::cout << "total size: " << sizeof(kIntegerValue) * kIntegersCount + kStringValue.size() * kStringsCount <<
     * std::endl;*/

    std::vector<Result> results;

    try {
        if (args.serializers.empty() || args.serializers.find("thrift-binary") != args.serializers.end()) {
            results.push_back(thrift_serialization_test(args.iterations, ThriftSerializationProto::Binary));
        }

        if (args.serializers.empty() || args.serializers.find("thrift-compact") != args.serializers.end()) {
            results.push_back(thrift_serialization_test(args.iterations, ThriftSerializationProto::Compact));
        }

        if (args.serializers.empty() || args.serializers.find("protobuf") != args.serializers.end()) {
            results.push_back(protobuf_serialization_test(args.iterations));
        }

        if (args.serializers.empty() || args.serializers.find("capnproto") != args.serializers.end()) {
            results.push_back(capnproto_serialization_test(args.iterations));
        }

        if (args.serializers.empty() || args.serializers.find("boost") != args.serializers.end()) {
            results.push_back(boost_serialization_test(args.iterations));
        }

        if (args.serializers.empty() || args.serializers.find("msgpack") != args.serializers.end()) {
            results.push_back(msgpack_serialization_test(args.iterations));
        }

        if (args.serializers.empty() || args.serializers.find("cereal") != args.serializers.end()) {
            results.push_back(cereal_serialization_test(args.iterations));
        }

        if (args.serializers.empty() || args.serializers.find("avro") != args.serializers.end()) {
            results.push_back(avro_serialization_test(args.iterations));
        }

        if (args.serializers.empty() || args.serializers.find("flatbuffers") != args.serializers.end()) {
            results.push_back(flatbuffers_serialization_test(args.iterations));
        }

        if (args.serializers.empty() || args.serializers.find("yas") != args.serializers.end()) {
            results.push_back(yas_serialization_test<yas::binary | yas::no_header>(args.iterations));
        }

        if (args.serializers.empty() || args.serializers.find("yas-compact") != args.serializers.end()) {
            results.push_back(yas_serialization_test<yas::binary | yas::no_header | yas::compacted>(args.iterations));
        }
    } catch (std::exception& exc) {
        std::cerr << "Error: " << exc.what() << std::endl;
        return EXIT_FAILURE;
    }

    print_results(args, results);

    google::protobuf::ShutdownProtobufLibrary();

    return EXIT_SUCCESS;
}
