#### [Thrift](http://thrift.apache.org/) vs. [Protobuf](https://code.google.com/p/protobuf/) vs. [Boost.Serialization](http://www.boost.org/libs/serialization) vs. [Msgpack](http://msgpack.org/) vs. [Cereal](http://uscilab.github.io/cereal/index.html) serialization/deserialization time test for C++.

#### Usage
1. Run all tests, run each serializer 100000 times.
```
$ ./test 100000
```
1. Run only protobuf serializer, run it 100000 times.
```
$ ./test 100000 protobuf
```
1. Run protobuf and cereal serializers only, run them 100000 times.
```
$ ./test 100000 protobuf cereal
```

#### Results

![Size](images/size.png)
![Time](images/time.png)