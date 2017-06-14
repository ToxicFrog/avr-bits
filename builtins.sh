#!/usr/bin/env bash
#
# If this gets called, we are in the bootstrap process. We need to generate the
# .dict and .impl files for each .nf file in builtins/.
#
# The trick here is that doing so requires a notforth compiler, and later files
# may depend on words defined in earlier files -- and if those words are written
# in C, we can't call them without recompiling.
#
# So the process is something like:
# - assume the makefile has already built nf-bootstrap
# - go down the list of .nf files
# - if we find one that's newer than the matching .dict/.impl files, run
#   nf-bootstrap foo.nf, regenerate builtins/all.{h,c}, then recompile nf-bootstrap
# - once this process is over, we have an nf-bootstrap incorporating all of the
#   .nf files, and builtins/all.{h,c} containing the most recent version

set -e

for nf in builtins/*.nf; do
  if [[ $nf -ot $nf.impl ]]; then
    echo "Up to date: $nf"
    continue
  fi
  echo "Rebuilding: $nf"
  (cd builtins; ../nf-bootstrap ../$nf)
  (echo "#ifdef ENABLE_BUILTINS"; cat builtins/*.nf.impl; echo "#endif") > builtins/all.c
  (echo "#ifdef ENABLE_BUILTINS"; cat builtins/*.nf.dict; echo "#endif") > builtins/all.h
  make --always-make --silent nf-bootstrap
done
