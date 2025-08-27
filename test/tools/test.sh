#!/bin/bash

if [[ "$(basename $(pwd))" != "tools" ]]; then
	echo Please run in \'tools\' directory!
	exit 1
fi

items=$(find .. -maxdepth 1 -name '*.yz')

COMPILER="../../amc"

BUILD_DIR="../.cache/amc"
UNITS_AUTO_DIR="test-units-auto"
UNITS_DIR="test-units"

mkdir -p ../build

compile() {
	local input="$1"
	local output="../build/$(basename $input .yz).out"

	local arg="$input --root-mod $(basename $input .yz) -o $output"
	local cmd="$COMPILER $arg"
	echo -e "==> \x1b[34mCompiling\x1b[0m: $(basename $input) $arg"
	#valgrind --tool=memcheck \
	#	--leak-check=full \
	#	--show-leak-kinds=all \
	#	--undef-value-errors=no \
	#	--log-file="$output".log \
	#	$cmd
	$cmd
	err=$?
	if [ $err -ne 0 ]; then
		echo -e "\x1b[31m--> ERROR\x1b[0m: Compiler stopped: $err!"
		echo -e "  > $cmd"
		echo -en "  > \x1b[34mHINT\x1b[0m: debug compiler? [y/n] "
		read ans
		if [ "$ans" = "y" ]; then
			gdb -args "$COMPILER" $arg
			exit 0
		else
			test_failed
		fi
	fi
}

test_all() {
	for item in $items; do
		test_src "$item"
	done
	for item in $(find $UNITS_DIR -name "*.sh"); do
		echo -e "==> \x1b[34mSpecial unit test begin\x1b[0m: $item $COMPILER -a"
		$item $COMPILER -a
		local err=$?
		if [ $err -ne 0 ]; then
			echo -e "\x1b[31m -> ERROR\x1b[0m: Special unit test stopped: $err!"
			exit 1
		fi
		echo -e "\x1b[32mDONE\x1b[0m: Special unit test end"
	done
}

test_failed() {
	echo -e "\x1b[31mTesting failed!\x1b[0m"
	exit 1
	#echo -en "Test other files? [y/n] "
	#read ans
	#[ "$ans" != "y" ] && return 1
}

test_src() {
	local input="$1"
	local unit="$UNITS_AUTO_DIR/test-$(basename $input).sh"
	rm -f "../build/$(basename $input .yz).out"
	compile "$input"
	if [ -f "$unit" ]; then
		test_unit "$unit" "$input" "../build/$(basename $input .yz).out"
	fi
	echo -e "\x1b[32mDONE\x1b[0m: $(basename $input)"
}

test_unit() {
	local script="$1"
	local src="$2"
	local bin="$3"
	local arg="$src $bin"
	echo -e " -> \x1b[33mUnit test begin\x1b[0m: $script $arg"
	$script $arg
	local err=$?
	if [ $err -ne 0 ]; then
		echo -e "\x1b[31m -> ERROR\x1b[0m: Unit test stopped: $err!"
		echo -e "  > $script $src $bin"
		exit 1
	fi
	echo -e " <- \x1b[33mUnit test end\x1b[0m"
}

case $1 in
	get_outputs) get_outputs ;;
	clean) clean_outputs ;;
	review) review ;;
	all|*) test_all ;;
esac
