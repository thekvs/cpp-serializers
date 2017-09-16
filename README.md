#### About

Compare various data serialization libraries for C++.

* [Thrift](http://thrift.apache.org/)
* [Protobuf](https://code.google.com/p/protobuf/)
* [Boost.Serialization](http://www.boost.org/libs/serialization)
* [Msgpack](http://msgpack.org/)
* [Cereal](http://uscilab.github.io/cereal/index.html)
* [Avro](http://avro.apache.org/)
* [Capnproto](https://capnproto.org/)
* [Flatbuffers](https://google.github.io/flatbuffers/)
* [YAS](https://github.com/niXman/yas)

#### Build
This project does not have any external library dependencies. All (boost, thrift etc.) needed libraries are downloaded
and built automatically, but you need enough free disk space to build all components. To build this project you need a compiler that supports
C++11 features. Project was tested with GCC 4.8.2 (Ubuntu 14.04).

```
$ git clone https://github.com/thekvs/cpp-serializers.git
$ mkdir /path/to/build-root/
$ cd /path/to/build-root/
$ cmake /path/to/cpp-serializers -DCMAKE_BUILD_TYPE=Release
$ make
```

#### Additional libraries required for building

* `zlib` not found, install it via
```
$ apt-get install zlib1g-dev
```
* `autoreconf` not found, install it via
```
$ apt-get install dh-autoreconf
```
* building `thrift` requires openssl dev files installed, install it via
```
$ apt-get install libssl-dev
```

#### Usage
* Test __all__ serializers, run each serializer 100000 times:
```
$ ./benchmark 100000
```
* Test only __protobuf__ serializer, run it 100000 times:
```
$ ./benchmark 100000 protobuf
```
* Test __protobuf__ and __cereal__ serializers only, run each of them 100000 times:
```
$ ./benchmark 100000 protobuf cereal
```

#### Results

Following results were obtained running 1000000 serialize-deserialize operations 50 times and then averaging results
on a typical desktop computer with Intel Core i5 processor running Ubuntu 14.04. Exact versions of libraries used are:

* thrift 0.10.0
* protobuf 3.1.0
* boost 1.62.0
* msgpack 2.1.3
* cereal 1.2.1
* avro 1.8.2
* capnproto 0.6.1
* flatbuffers 1.7.1
* YAS 5.0.0

| serializer     | object's size | avg. total time |
| -------------- | ------------- | --------------- |
| thrift-binary  | 17017         | 13335           |
| thrift-compact | 13378         | 33205           |
| protobuf       | 16116         | 24717           |
| boost          | 17470         | 21760           |
| msgpack        | 13402         | 33815           |
| cereal         | 17416         | 11031           |
| avro           | 16384         | 44187           |
| yas            | 17416         | 3223            |
| yas-compact    | 13553         | 21481           |

###### Size

![Size](images/size.png)

###### Time

![Time](images/time.png)

For capnproto and flatbuffers since they already store data in a "serialized" form and serialization basically means getting pointer
to the internal storage, we measure full __build__/serialize/deserialize cycle. In the case of other libraries we measure
serialize/deserialize cycle of the already built data structure.

| serializer     | object's size | avg. total time |
| -------------- | ------------- | --------------- |
| capnproto      | 17768         | 4849            |
| flatbuffers    | 17632         | 12520           |

![Time](images/time2.png)

Size measured in bytes, time measured in milliseconds.

