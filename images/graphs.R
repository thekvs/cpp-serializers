library(ggplot2)

names.size <- c("thrift-binary", "thrift-compact", "protobuf", "boost", "msgpack", "cereal", "avro", "capnproto", "flatbuffers", "yas", "yas-compact")
names.time <- c("thrift-binary", "thrift-compact", "protobuf", "boost", "msgpack", "cereal", "avro", "yas", "yas-compact")
names.time2 <- c("capnproto", "flatbuffers")
# data from the 1000000 simulations
# c=" "; for t in thrift-binary thrift-compact protobuf boost msgpack cereal avro capnproto flatbuffers yas yas-compact; do ${BENCHMARK} -i 1 -s $t | grep Size | awk "{printf \"%s%d # %s\n\", \"$c\", \$3, \"$t\"}"; c=","; done
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
# c=" "; for t in thrift-binary thrift-compact protobuf boost msgpack cereal avro yas yas-compact; do rm -f /tmp/$t.time; for i in `seq 1 ${RESTARTS}`; do ${BENCHMARK} -i ${ITERATIONS} -s $t | grep Time | awk '{print $3}' >>/tmp/$t.time; done; awk "{ sum += \$1 } END {printf \"%s%f # %s\n\", \"$c\", sum/50, \"$t\"}" /tmp/$t.time; c=","; done
time <- c(
     13795.100000 # thrift-binary
    ,41831.280000 # thrift-compact
    ,30041.420000 # protobuf
    ,14231.320000 # boost
    ,31668.700000 # msgpack
    ,14203.780000 # cereal
    ,55352.740000 # avro
    ,3307.360000 # yas
    ,28981.900000 # yas-compact
)
# c=" "; for t in capnproto flatbuffers; do rm -f /tmp/$t.time; for i in `seq 1 ${RESTARTS}`; do ${BENCHMARK} -i ${ITERATIONS} -s $t | grep Time | awk '{print $3}' >>/tmp/$t.time; done; awk "{ sum += \$1 } END {printf \"%s%f # %s\n\", \"$c\", sum/50, \"$t\"}" /tmp/$t.time; c=","; done
time2 <- c(
     4512.320000 # capnproto
    ,5821.920000 # flatbuffers
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
