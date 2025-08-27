#!/bin/sh

if [ -n "$2" ]; then
	cd ../module
else
	cd ../../module
fi
make

output="$(./main)"
ret=$?
echo $ret

if [ $ret -ne 0 ]; then
	exit 1
fi

if [ "$output" != "Hello world!" ]; then
	exit 1
fi
