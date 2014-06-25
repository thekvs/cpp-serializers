library(ggplot2)

name <- c("thrift", "protobuf", "boost", "msgpack", "cereal")
# data from the 1000000 simulations
size <- c(17017, 12571, 17470, 11902, 17416)
# for t in thrift protobuf boost msgpack cereal; do for i in `seq 1 50`; do ./test 1000000 $t | grep time | awk '{print $4}' >>/tmp/$t.time; done; echo $t; done
time <- c(24490, 21110, 22700, 18470, 11020)

qplot(factor(name), y=size, geom="bar", fill=factor(name), stat="identity")
qplot(factor(name), y=time, geom="bar", fill=factor(name), stat="identity")
