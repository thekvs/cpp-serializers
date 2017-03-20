
#include "yas/record.hpp"

namespace yas_test {

void
to_string(const Record &record, std::string &data)
{
    yas::mem_ostream os;
    yas::binary_oarchive<yas::mem_ostream, flags> oa(os);
    oa & record;

    auto buf = os.get_intrusive_buffer();
    data.assign(buf.data, buf.size);
}

void
from_string(Record &record, const std::string &data)
{
    yas::mem_istream is(data.c_str(), data.size());
    yas::binary_iarchive<yas::mem_istream, flags> ia(is);
    ia & record;
}

} // namespace
