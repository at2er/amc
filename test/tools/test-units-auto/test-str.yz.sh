#!/bin/bash

src="$1"
bin="$2"

r="$($bin)"

echo "$r"

if [ "$r" != "Hello world!" ]; then
	exit 1
fi
