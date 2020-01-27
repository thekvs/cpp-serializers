#!/bin/bash
ITERATIONS=1000000
RESTARTS=50
BENCHMARK="../build/benchmark"

if [ ! -f ${BENCHMARK} ]; then
  echo "Error: executable build/benchmark not found"
  exit 1
fi

echo "-- Generating data for images/graphs.R..."
echo "-- Substitute lines 9 through 19 with this:"
c=" "
for t in thrift-binary thrift-compact protobuf boost msgpack cereal avro capnproto flatbuffers yas yas-compact;
do
  ${BENCHMARK} -i 1 -s $t | grep Size | awk "{printf \"%s%d # %s\n\", \"$c\", \$3, \"$t\"}"
  c=","
done

echo ""
echo "-- Substitute lines 23 through 31 with this:"
c=" "
for t in thrift-binary thrift-compact protobuf boost msgpack cereal avro yas yas-compact;
do
  rm -f /tmp/$t.time
  for i in `seq 1 ${RESTARTS}`
  do 
    ${BENCHMARK} -i ${ITERATIONS} -s $t | grep Time | awk '{print $3}' >>/tmp/$t.time
  done;
  awk "{ sum += \$1 } END {printf \"%s%f # %s\n\", \"$c\", sum/50, \"$t\"}" /tmp/$t.time
  c=","
done

echo ""
echo "-- Substitute lines 35 and 36 with this:"
c=" "
for t in capnproto flatbuffers;
do
  rm -f /tmp/$t.time;
  for i in `seq 1 ${RESTARTS}`;
  do 
    ${BENCHMARK} -i ${ITERATIONS} -s $t | grep Time | awk '{print $3}' >>/tmp/$t.time
  done;
  awk "{ sum += \$1 } END {printf \"%s%f # %s\n\", \"$c\", sum/50, \"$t\"}" /tmp/$t.time
  c=","
done

echo "-- Run the following command to generate the new images:"
echo "   R CMD BATCH images/graphs.R"
