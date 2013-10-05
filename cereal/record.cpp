#include "cereal/record.hpp"

namespace cereal_test {

void
to_string(const Record &record, std::string &data)
{
    std::ostringstream stream;
    cereal::BinaryOutputArchive archive(stream);
    archive(record);
    data = stream.str();
}

void
from_string(Record &record, const std::string &data)
{
    std::stringstream stream(data);
    cereal::BinaryInputArchive archive(stream);
    archive(record);
}

} // namespace
