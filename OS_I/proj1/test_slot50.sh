#! /bin/bash
# to be improved automatically, this is too time-consuming for man.

local1=1

while test $local1 -le 20
do
    echo "___________________________________________________________"
    ./linux_donut_loop.sh $local1 >>  test_50.log &
    local1=`expr $local + 10`
    echo "___________________________________________________________"
done
