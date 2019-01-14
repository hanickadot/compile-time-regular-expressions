#!/usr/bin/env bash
if echo "" | $1 -std=c++2a -E -x c++ - 2>/dev/null 1>/dev/null; then
	VERSION=`$1 -v 2>&1 | grep "gcc" | grep "experimental"`
	if [ -z "$VERSION" ]; then
		echo "-std=c++2a";
	else
		echo "-std=c++2a";
	fi
else
	echo "-std=c++17";
fi
