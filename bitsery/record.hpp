#ifndef __BITSERY_RECORD_HPP_INCLUDED__
#define __BITSERY_RECORD_HPP_INCLUDED__

#include <vector>
#include <string>
#include <bitsery/bitsery.h>
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
};

void bitsery_serialize(const Record &record, std::vector<uint8_t> &data) {
    bitsery::BufferWriter bw{data};
    bitsery::Serializer ser{bw};
    ser.object(record, [&ser](Record &o) {
        ser.container8b(o.ids, 100000000);
        ser.container(o.strings, 100000000, [&ser](std::string& str) {
            //for(auto& str:o.strings) 
            ser.text1b(str, 10000000);        
        });
    });
    bw.flush();
    auto range = bw.getWrittenRange();
    //data.resize(std::distance(range.begin(), range.end()));    
}

void bitsery_deserialize(Record &record, std::vector<uint8_t> &data) {
    bitsery::BufferReader br(std::addressof(*data.begin()), std::addressof(*data.end()));
    bitsery::Deserializer des{br};
    des.object(record, [&des](Record &o) {
        des.container8b(o.ids, 100000000);
        des.container(o.strings, 100000000, [&des](std::string& str) {
            //for(auto& str:o.strings) 
            des.text1b(str, 10000000);
        });
    });
};

} // namespace

#endif
