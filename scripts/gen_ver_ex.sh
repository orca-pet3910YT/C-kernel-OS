#!/bin/bash
cat > include/generated/__BLD.h <<EOF
#ifndef BUILD_NUMBER
#define BUILD_NUMBER $(cat .version)
#endif
EOF
