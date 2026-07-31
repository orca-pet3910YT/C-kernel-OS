#!/bin/bash

if [ $# -ne 4 ]; then
	echo "$0: needs 4 arguments"
	exit 1
fi

gen_file="include/generated/config.h"

echo "#pragma once" > $gen_file
while IFS= read -r line; do
	if [[ "$line" == "# CONFIG_"* ]]; then
		key=$(echo "$line" | awk '{print $2}')
		key=${key#CONFIG_}
		echo "#define CONFIG_$key 0" >> $gen_file
		continue
	fi
done < .config
while IFS='=' read -r key value; do
	[[ "$key" == \#* ]] && continue
	[ -z "$key" ] && continue
	key=${key#CONFIG_}
	if [ "$value" = "y" ]; then
		echo "#define CONFIG_$key 1" >> $gen_file
	elif [[ "$value" == \"*\" ]]; then
		echo "#define CONFIG_$key $value" >> $gen_file
	else
		echo "#define CONFIG_$key $value" >> $gen_file
	fi
done < .config

cat >> $gen_file <<EOF
#define CONFIG_KERNEL_MAJOR $1
#define CONFIG_KERNEL_MINOR $2
#define CONFIG_KERNEL_PATCH $3
#define CONFIG_KERNEL_ADDITIONAL_MAKEFILE "$4"
#define VER_STR(x) #x
#define XVER_STR(x) VER_STR(x)
#define CONFIG_KERNEL_VER XVER_STR(CONFIG_KERNEL_MAJOR) "." XVER_STR(CONFIG_KERNEL_MINOR) "." XVER_STR(CONFIG_KERNEL_PATCH) CONFIG_KERNEL_ADDITIONAL_MAKEFILE CONFIG_ADDITIONAL_VER
#define CONFIG_KERNEL_VER_STR "CkOS " CONFIG_KERNEL_VER " for i386 (x86-32)"
#define CONFIG_OS_NAME "CkOS"
EOF
