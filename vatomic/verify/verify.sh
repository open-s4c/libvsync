#!/bin/sh
set -eu

if [ $# -ne 2 ]; then
	echo "Usage: $0 FUNCTION_NAME ARCH"
	exit 1
fi

FUNC="$1"
ARCH="$2"

OUT="out/$ARCH"
OUT_RETRY="out_retry/$ARCH"

verify() {
	phase="$1"
	outdir="$2"
	echo "Verifying $FUNC on $ARCH (phase $phase, outdir=$outdir)"
	boogie /proverOpt:SOLVER=z3 \
		"$outdir/$FUNC"/*.bpl \
		./boogie/auxiliary.bpl \
		"./$ARCH/library.bpl"
}

# phase 1
if verify 1 "$OUT" | tee /dev/stderr | grep -q "0 errors"; then
	echo "$FUNC on $ARCH passed phase 1"
	exit 0
fi

# phase 2
echo "Phase 1 failed, retrying with heavy verification..."
if verify 2 "$OUT_RETRY" | tee /dev/stderr | grep -q "0 errors"; then
	echo "$FUNC on $ARCH passed phase 2"
	exit 0
else
	echo "$FUNC on $ARCH failed verification"
	exit 1
fi
