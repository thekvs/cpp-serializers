
#ifndef __YAS_RECORD_HPP_INCLUDED__
#define __YAS_RECORD_HPP_INCLUDED__

#include <vector>
#include <string>

#include <stdint.h>

#include <yas/mem_streams.hpp>
#include <yas/binary_iarchive.hpp>
#include <yas/binary_oarchive.hpp>
#include <yas/std_types.hpp>

namespace yas_test {

enum { flags = yas::binary|yas::no_header|yas::seq_size_32 };

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

    template<typename Archive>
    void serialize(Archive &ar)
    {
        ar & ids & strings;
    }
};

void to_string(const Record &record, std::string &data);
void from_string(Record &record, const std::string &data);

} // namespace

#endif
