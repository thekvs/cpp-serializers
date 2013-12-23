library(ggplot2)

name <- c("thrift", "protobuf", "boost", "msgpack", "cereal")
# data from the 1000000 simulations
size <- c(17017, 12800, 17470, 11907, 17416)
time <- c(24047, 17952, 24100, 24653, 10613)

qplot(factor(name), y=size, geom="bar", fill=factor(name), stat="identity")
qplot(factor(name), y=time, geom="bar", fill=factor(name), stat="identity")
