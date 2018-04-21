
#ifndef __ZPP_RECORD_HPP_INCLUDED__
#define __ZPP_RECORD_HPP_INCLUDED__

#include <vector>
#include <string>

#include <stdint.h>

#include "zpp/serializer.h"

namespace zpp_test {

typedef std::vector<int64_t>     Integers;
typedef std::vector<std::string> Strings;

struct Record {

    Integers ids;
    Strings  strings;

    bool operator==(const Record &other) {
        return (ids == other.ids && strings == other.strings);
    }

    bool operator!=(const Record &other) {
        return !(*this == other);
    }

    template <typename Archive, typename Self>
    static void serialize(Archive & archive, Self & self)
    {
        archive(self.ids, self.strings);
    }
};

inline void to_string(const Record &record, std::string &string_data) {
    std::vector<unsigned char> data;
    zpp::serializer::memory_output_archive archive(data);
    archive(record);
    string_data.assign(reinterpret_cast<const char *>(data.data()), data.size());
}

inline void from_string(Record &record, const std::string &data) {
    zpp::serializer::memory_view_input_archive archive(data.data(), data.size());
    archive(record);
}

} // namespace

#endif
