/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef _HOME_PKOK_CPP_SERIALIZERS_AVRO_RECORD_HPP_2710640864__H_
#define _HOME_PKOK_CPP_SERIALIZERS_AVRO_RECORD_HPP_2710640864__H_


#include <sstream>
#include "boost/any.hpp"
#include "avro/Specific.hh"
#include "avro/Encoder.hh"
#include "avro/Decoder.hh"

namespace avro_test {
struct Record {
    std::vector<int64_t > ids;
    std::vector<std::string > strings;
    Record() :
        ids(std::vector<int64_t >()),
        strings(std::vector<std::string >())
        { }
};

}
namespace avro {
template<> struct codec_traits<avro_test::Record> {
    static void encode(Encoder& e, const avro_test::Record& v) {
        avro::encode(e, v.ids);
        avro::encode(e, v.strings);
    }
    static void decode(Decoder& d, avro_test::Record& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.ids);
                    break;
                case 1:
                    avro::decode(d, v.strings);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.ids);
            avro::decode(d, v.strings);
        }
    }
};

}
#endif
