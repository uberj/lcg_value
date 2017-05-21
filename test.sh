#!/bin/bash

make clean 
make
if [ $? -ne 0 ]; then
    echo FAIL
	exit 1
fi

./lcg-state-forward-orig-with-modification 1 1 100 > run-orig.txt && ./lcg-state-forward 1 1 100 > run-wip.txt

test=$(diff run-orig.txt run-wip.txt)
if [[ $test ]]; then
	echo "FAIL! WIP has drifted"
    echo $test
else
	echo "PASS! Parody in place"
fi
