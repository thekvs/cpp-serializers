#ifndef __MSGPACK_RECORD_HPP_INCLUDED__
#define __MSGPACK_RECORD_HPP_INCLUDED__

#include <vector>
#include <string>

#include <stdint.h>

#include <msgpack.hpp>

namespace msgpack_test {

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

    MSGPACK_DEFINE(ids, strings);
};

} // namespace

#endif
