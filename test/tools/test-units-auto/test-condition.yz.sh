#!/bin/bash

src="$1"
bin="$2"

$bin

ret=$?
echo $ret

if [ $ret -ne 5 ]; then
	exit 5
fi
