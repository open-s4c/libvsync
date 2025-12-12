#!/bin/sh
set -eu

if [ $# -ne 2 ]; then
  echo "Usage: $0 FUNCTION_NAME ARCH"
  exit 1
fi

FUNC="$1"
ARCH="$2"

case "${ARCH}" in
	armv8)
		ARCHS="./armv8/atomics.s"
		;;
	riscv)
		ARCHS="./riscv/atomics.s"
		;;
	*)
		echo "Unknown architecture: $ARCH"
		exit 1
		;;
esac

UNROLL=
OUT="out/$ARCH"
OUT_RETRY="out_retry/$ARCH"
TMPFUNCS=$(mktemp)
echo "$FUNC" > "$TMPFUNCS"

compile() {
	outdir="$1"
	unroll="$2"
	if $unroll; then
		UNROLL="--unroll"
	fi
	echo "Generating $FUNC for $ARCH (unroll=$unroll, outdir=$outdir)"
	cargo run --quiet -- \
		--input "${ARCHS}" \
		--functions "$TMPFUNCS" \
		--templates ./boogie/templates/ \
		--directory "$outdir" \
		--arch "$ARCH" $UNROLL
}

# generate for both phases
compile "$OUT" "false"
compile "$OUT_RETRY" "true"

rm "$TMPFUNCS"
