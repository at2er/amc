#!/bin/bash

src="$1"
bin="$2"

$bin

ret=$?
echo $ret

if [ $ret -ne 0 ]; then
	exit 1
fi
