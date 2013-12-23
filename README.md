#### [Thrift](http://thrift.apache.org/) vs. [Protobuf](https://code.google.com/p/protobuf/) vs. [Boost.Serialization](http://www.boost.org/libs/serialization) vs. [Msgpack](http://msgpack.org/) vs. [Cereal](http://uscilab.github.io/cereal/index.html) serialization/deserialization time test for C++.

#### Build
This project does not have external library dependencies. All (boost, thrift etc.) needed libraries are downloaded
and built automatically, but you need enough free disk space. To build this project you need a compiler that supports
C++11 features. Project was tested with GCC 4.7 (ubuntu 13.04).

```
$ git clone https://github.com/thekvs/cpp-serializers.git
$ mkdir /path/to/build-root/
$ cd /path/to/build-root/
$ cmake /path/to/cpp-serializers -DCMAKE_BUILD_TYPE=Release
$ make
```

#### Usage
1. Test __all__ serializers, run each serializer 100000 times:
```
$ ./test 100000
```
1. Test only __protobuf__ serializer, run it 100000 times:
```
$ ./test 100000 protobuf
```
1. Test __protobuf__ and __cereal__ serializers only, run each of them 100000 times:
```
$ ./test 100000 protobuf cereal
```

#### Results

Following results were obtained running 1000000 serialize-deserialize operations on typical desktop computer
with Intel Core i5 processor running Ubuntu 13.04. Exact versions of libraries used are:

* boost 1.55.0
* thrift 0.9.1
* msgpack 0.5.8
* protobuf 2.5.0
* cereal 0.9.1

| serializer | object's size | total time |
| ---------- | ------------- | ---------- |
| thrift     | 17017         | 24047      |
| protobuf   | 12800         | 17952      |
| boost      | 17470         | 24100      |
| msgpack    | 11907         | 24653      |
| cereal     | 17416         | 10613      |

Size mesuared in bytes, time mesuared in milliseconds.

##### Graphical representations

###### Size

![Size](images/size.png)

###### Time

![Time](images/time.png)
