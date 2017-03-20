library(ggplot2)

names.size <- c("thrift-binary", "thrift-compact", "protobuf", "boost", "msgpack", "cereal", "avro", "capnproto", "flatbuffers", "yas")
names.time <- c("thrift-binary", "thrift-compact", "protobuf", "boost", "msgpack", "cereal", "avro", "yas")
names.time2 <- c("capnproto", "flatbuffers")
# data from the 1000000 simulations
# for t in thrift-binary thrift-compact protobuf boost msgpack cereal avro yas; do echo -n "$t: "; ./benchmark 1 $t | grep size | awk '{print $4}'; done
size <- c(
     17017
    ,11597
    ,11574
    ,17470
    ,11802
    ,17416
    ,12288
    ,17768
    ,17632
    ,17015
)
# for t in thrift-binary thrift-compact protobuf boost msgpack cereal avro yas; do rm -f /tmp/$t.time; echo -n "$t: "; for i in `seq 1 50`; do ./benchmark 1000000 $t | grep time | awk '{print $4}' >>/tmp/$t.time; done; awk '{ sum += $1 } END { print sum/50}' /tmp/$t.time; done
time <- c(
     13179 # thrift-binary
    ,25562 # thrift-compact
    ,22468 # protobuf
    ,21405 # boost
    ,27805 # msgpack
    ,10654 # cereal
    ,31231 # avro
    ,4945  # yas
)
time2 <- c(4396, 12494)

data.size <- as.data.frame(list(serializer = names.size, size = size))
data.time <- as.data.frame(list(serializer = names.time, time = time))
data.time2 <- as.data.frame(list(serializer = names.time2, time = time2))

ssize <- ggplot(data.size, aes(x = as.factor(serializer), y = as.factor(size), fill = serializer)) +
    geom_bar(stat = "identity") +
    xlab("serializer") +
    ylab("size") +
    theme(axis.text.x = element_text(angle = 90, hjust = 1))
png(filename="size.png", width = 800, height = 600)
plot(ssize)
dev.off()

stime <- ggplot(data.time, aes(x = as.factor(serializer), y = as.factor(time), fill = serializer)) +
    geom_bar(stat = "identity") +
    xlab("serializer") +
    ylab("time") +
    theme(axis.text.x = element_text(angle = 90, hjust = 1))
png(filename="time.png", width = 800, height = 600)
plot(stime)
dev.off()

stime2 <- ggplot(data.time2, aes(x = as.factor(serializer), y = as.factor(time), fill = serializer)) +
    geom_bar(stat = "identity") +
    xlab("serializer") +
    ylab("time") +
    theme(axis.text.x = element_text(angle = 90, hjust = 1))
png(filename="time2.png", width = 800, height = 600)
plot(stime2)
dev.off()
