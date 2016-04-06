library(ggplot2)

names.size <- c("thrift-binary", "thrift-compact", "protobuf", "boost", "msgpack", "cereal", "avro", "capnproto", "flatbuffers")
names.time <- c("thrift-binary", "thrift-compact", "protobuf", "boost", "msgpack", "cereal", "avro")
names.time2 <- c("capnproto", "flatbuffers")
# data from the 1000000 simulations
# for t in thrift-binary thrift-compact protobuf boost msgpack cereal avro; do echo -n "$t: "; ./benchmark 1 $t | grep size | awk '{print $4}'; done
size <- c(17017, 11597, 12571, 17470, 11902, 17416, 12288, 17768, 17632)
# for t in thrift-binary thrift-compact protobuf boost msgpack cereal avro; do rm -f /tmp/$t.time; echo -n "$t: "; for i in `seq 1 50`; do ./benchmark 1000000 $t | grep time | awk '{print $4}' >>/tmp/$t.time; done; awk '{ sum += $1 } END { print sum/50}' /tmp/$t.time; done
time <- c(13763, 27017, 21034, 22945, 23560, 10688, 31750)
time2 <- c(4396, 12494)

data.size <- as.data.frame(list(serializer = names.size, size = size))
data.time <- as.data.frame(list(serializer = names.time, time = time))
data.time2 <- as.data.frame(list(serializer = names.time2, time = time2))

ggplot(data.size, aes(x = as.factor(serializer), y = as.factor(size), fill = serializer)) +
    geom_bar(stat = "identity") +
    xlab("serializer") +
    ylab("size") +
    theme(axis.text.x = element_text(angle = 90, hjust = 1))

ggplot(data.time, aes(x = as.factor(serializer), y = as.factor(time), fill = serializer)) +
    geom_bar(stat = "identity") +
    xlab("serializer") +
    ylab("time") +
    theme(axis.text.x = element_text(angle = 90, hjust = 1))

ggplot(data.time2, aes(x = as.factor(serializer), y = as.factor(time), fill = serializer)) +
    geom_bar(stat = "identity") +
    xlab("serializer") +
    ylab("time") +
    theme(axis.text.x = element_text(angle = 90, hjust = 1))

