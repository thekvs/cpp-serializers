#ifndef __BITSERY_RECORD_HPP_INCLUDED__
#define __BITSERY_RECORD_HPP_INCLUDED__

#include <vector>
#include <string>
#include <bitsery/bitsery.h>
#include <bitsery/adapter/buffer.h>

#include <bitsery/flexible.h>
#include <bitsery/flexible/vector.h>
#include <bitsery/flexible/string.h>

#include <memory>

namespace bitsery_test {

typedef std::vector<int64_t>     Integers;
typedef std::vector<std::string> Strings;

class Record {
public:

    Integers ids;
    Strings  strings;

    bool operator==(const Record &other) {
        return (ids == other.ids && strings == other.strings);
    }

    bool operator!=(const Record &other) {
        return !(*this == other);
    }

    template <typename S>
    void serialize(S& s) {
        //maxSize - enforces container max size
//        s.archive(
//                bitsery::maxSize(ids, 1000000),
//                bitsery::maxSize(strings, 1000000));
        //optionally you can use like this, without maxSize restriction
        s.archive(ids,strings);
    }
};

using OutputAdapter = bitsery::OutputBufferAdapter<std::vector<uint8_t>>;
using InputAdapter = bitsery::InputBufferAdapter<std::vector<uint8_t>>;

size_t bitsery_serialize(const Record &record, std::vector<uint8_t> &data) {
    return bitsery::quickSerialization<OutputAdapter>({data}, record);
}

void bitsery_deserialize(Record &record, std::vector<uint8_t> &data, size_t writtenSize) {
    bitsery::quickDeserialization<InputAdapter>(InputAdapter{data.begin(), writtenSize}, record);
};

} // namespace

#endif
