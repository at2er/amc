#!/bin/sh

if [ -n "$2" ]; then
	cd ../complexc
else
	cd ../../complexc
fi
make

output="$(./main)"
ret=$?
echo $output
echo $ret

if [ $ret -ne 1 ]; then
	exit 1
fi

if [ "$output" != "Hello world!" ]; then
	exit 1
fi
