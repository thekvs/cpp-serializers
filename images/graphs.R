library(ggplot2)

names.size <- c("thrift-binary", "thrift-compact", "protobuf", "boost", "msgpack", "cereal", "avro", "capnproto", "flatbuffers", "yas", "yas-compact")
names.time <- c("thrift-binary", "thrift-compact", "protobuf", "boost", "msgpack", "cereal", "avro", "yas", "yas-compact")
names.time2 <- c("capnproto", "flatbuffers")
# data from the 1000000 simulations
# for t in thrift-binary thrift-compact protobuf boost msgpack cereal avro yas yas-compact; do echo -n "$t: "; ./benchmark 1 $t | grep size | awk '{print $4}'; done
size <- c(
     17017 # thrift-binary
    ,13378 # thrift-compact
    ,16116 # protobuf
    ,17470 # boost
    ,13402 # msgpack
    ,17416 # cereal
    ,16384 # avro
    ,17768 # capnproto
    ,17632 # flatbuffers
    ,17416 # yas
    ,13321 # yas-compact
)
# for t in thrift-binary thrift-compact protobuf boost msgpack cereal avro yas yas-compact; do rm -f /tmp/$t.time; echo -n "$t: "; for i in `seq 1 50`; do ./benchmark 1000000 $t | grep time | awk '{print $4}' >>/tmp/$t.time; done; awk '{ sum += $1 } END { print sum/50}' /tmp/$t.time; done
time <- c(
     1190.22 # thrift-binary
    ,3474.32 # thrift-compact
    ,2312.78 # protobuf
    ,1195.04 # boost
    ,2560.6  # msgpack
    ,1052.46 # cereal
    ,4488.18 # avro
    ,302.7   # yas
    ,2063.34 # yas-compact
)
time2 <- c(
     400.98 # capnproto
    ,491.5  # flatbuffers
)

data.size <- as.data.frame(list(serializer = names.size, size = size))
data.time <- as.data.frame(list(serializer = names.time, time = time))
data.time2 <- as.data.frame(list(serializer = names.time2, time = time2))

ssize <- ggplot(data.size, aes(x = as.factor(serializer), y = size, fill = serializer)) +
    geom_bar(stat = "identity") +
    xlab("serializer") +
    ylab("size") +
    theme(axis.text.x = element_text(angle = 90, hjust = 1))
png(filename="size.png", width = 800, height = 600)
plot(ssize)
dev.off()

stime <- ggplot(data.time, aes(x = as.factor(serializer), y = time, fill = serializer)) +
    geom_bar(stat = "identity") +
    xlab("serializer") +
    ylab("time") +
    theme(axis.text.x = element_text(angle = 90, hjust = 1))
png(filename="time.png", width = 800, height = 600)
plot(stime)
dev.off()

stime2 <- ggplot(data.time2, aes(x = as.factor(serializer), y = time, fill = serializer)) +
    geom_bar(stat = "identity") +
    xlab("serializer") +
    ylab("time") +
    theme(axis.text.x = element_text(angle = 90, hjust = 1))
png(filename="time2.png", width = 800, height = 600)
plot(stime2)
dev.off()
