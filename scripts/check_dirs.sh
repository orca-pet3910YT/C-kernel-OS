#!/bin/bash
if [[ -d include && -d src && -f Makefile && -f kernel.ld ]]; then
	exit 0
fi
exit 1
