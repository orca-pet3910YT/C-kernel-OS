#!/bin/bash
date_bld=$(date)
echo "   GEN  include/generated/__GENVER.h"
rm -f include/generated/__GENVER.h
tr -d '\r' > include/generated/__GENVER.h <<EOF
#ifndef CKOS_BLD
#define CKOS_BLD "$(whoami)@$(hostname) at ${date_bld} on $(uname -s)"
#endif
EOF
