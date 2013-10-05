#ifndef __CEREAL_RECORD_HPP_INCLUDED__
#define __CEREAL_RECORD_HPP_INCLUDED__

#include <vector>
#include <string>
#include <sstream>

#include <stdint.h>

#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>

namespace cereal_test {

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

private:

    friend class cereal::access;

    template<typename Archive>
    void serialize(Archive &archive)
    {
        archive(ids, strings);
    }
};

void to_string(const Record &record, std::string &data);
void from_string(Record &record, const std::string &data);

} // namespace

#endif
