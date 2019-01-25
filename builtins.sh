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

FILES=( $(egrep -v '^#' builtins/LIST ) )
DIRTY=0

for nf in ${FILES[@]}; do
  if [[ builtins/$nf -ot build/$nf.impl ]] && ! (( DIRTY )); then
    echo "Up to date: $nf"
    continue
  fi
  DIRTY=1
  echo "Rebuilding: $nf"
  (cd build; ../nf-bootstrap ../builtins/$nf) || {
    echo "Compilation failed, cleaning up..."
    rm -f build/$nf.impl
    exit 1
  }
  {
    echo "#ifdef ENABLE_BUILTINS"
    for file in ${FILES[@]}; do
      if [[ -f build/$file.impl ]]; then
        cat build/$file.impl
      fi
    done
    echo "#endif"
  } > builtins/all.c
  make --always-make --silent nf-bootstrap
done
