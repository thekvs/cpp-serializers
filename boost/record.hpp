#ifndef __RECORD_HPP_INCLUDED__
#define __RECORD_HPP_INCLUDED__

#include <vector>
#include <string>
#include <sstream>

#include <stdint.h>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/serialization/version.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/tracking.hpp>

namespace boost_test {

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

    friend class boost::serialization::access;

    template<typename Archive>
    void serialize(Archive &ar, const unsigned int)
    {
        ar & ids;
        ar & strings;
    }
};

void to_string(const Record &record, std::string &data);
void from_string(Record &record, const std::string &data);

} // namespace

#endif
