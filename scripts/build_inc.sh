#!/bin/bash
VER=".version"
if [[ ! -f "$VER" ]]; then
	echo 0 > "$VER"
fi
count=$(<"$VER")
((count++))
echo -n $count > $VER
echo "Build #$count"
