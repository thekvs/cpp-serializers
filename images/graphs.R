library(ggplot2)

name <- c("thrift-binary", "thrift-compact", "protobuf", "boost", "msgpack", "cereal", "avro")
# data from the 1000000 simulations
# for t in thrift-binary thrift-compact protobuf boost msgpack cereal avro; do echo -n "$t: "; ./benchmark 1 $t | grep size | awk '{print $4}'; done
size <- c(17017, 11597, 12571, 17470, 11902, 17416, 12288)
# for t in thrift-binary thrift-compact protobuf boost msgpack cereal avro; do rm -f /tmp/$t.time; echo -n "$t: "; for i in `seq 1 50`; do ./benchmark 1000000 $t | grep time | awk '{print $4}' >>/tmp/$t.time; done; awk '{ sum += $1 } END { print sum/50}' /tmp/$t.time; done
time <- c(13763, 27017, 21034, 22945, 23560, 10688, 31750)

qplot(factor(name), y=size, geom="bar", fill=factor(name), stat="identity") + xlab("serializer")
qplot(factor(name), y=time, geom="bar", fill=factor(name), stat="identity") + xlab("serializer")
