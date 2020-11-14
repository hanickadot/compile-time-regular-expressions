#!/usr/bin/env bash

git bisect reset
git bisect start
git bisect bad 
git bisect good $1

previous_hash=""
current_hash=`git rev-parse HEAD`

while [ "$previous_hash" != "$current_hash" ]
do

if clang++ -std=c++20 problem.cpp -c -o problem.o; then
	rm -f problem.o
	git bisect good
else
	git bisect bad
fi

previous_hash="$current_hash"
current_hash=`git rev-parse HEAD`

done

