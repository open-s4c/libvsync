#!/bin/sh
# ensure-cmd.sh: Fetch/build command and print the absolute path to the binary.
# ------------------------------------------------------------------------------
# Copyright (c) 2025 Diogo Behrens
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
# REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
# AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
# INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
# LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
# OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
# PERFORMANCE OF THIS SOFTWARE.
# ------------------------------------------------------------------------------
# You can copy this file into another project to ensure the command is installed
# automatically and adapt or remove the license above.
#
# Usage:
#     ensure-cmd.sh [OPTIONS] <program> <version>
#
# See ensure-cmd.sh for details on the usage.
#
# This snippet can be used in a CMake file to find the program or fetch/build
# it, setting the path to the program in variable PROGRAM_PATH:
#
#   execute_process(
#     COMMAND "path/to/ensure-cmd.sh" -q
#             --workdir "${CMAKE_BINARY_DIR}"
#             --url "${URL}"
#             --sha256 "${SHA256}"
#             "${PROGRAM}" "${VERSION}"
#     OUTPUT_VARIABLE PROGRAM_PATH
#     OUTPUT_STRIP_TRAILING_WHITESPACE
#     RESULT_VARIABLE ENSURE_RC)
#   if(NOT ENSURE_RC EQUAL 0)
#       message(FATAL_ERROR "Failed to ensure ${VERSION} is available")
#   endif()
#
set -eu

usage() {
	cat >&2 <<EOF
Usage: ${0##*/} [OPTIONS] <program> <version>

Ensures <program> exists and matches <version>. Without --url only existing
installations are validated. DIR defaults to the current working directory.

Options:
  -q              Quiet output, only print path to program
  --workdir DIR   Directory used to cache/download/build (default: \$PWD)
  --url URL       Tarball URL for requested version (required to download/build)
  --sha256 HASH   Optional SHA256 checksum (format: HEX or SHA256=HEX)
  -h, --help      Show this help text and exit
EOF
}

log() {
	if [ "$QUIET" != true ]; then
		printf '%s\n' "$*" >&2
	fi
}

warn() {
	if [ "$QUIET" != true ]; then
		printf 'warning: %s\n' "$*" >&2
	fi
}

output() {
	if [ "$QUIET" = true ]; then
		cat <&0 > /dev/null
	else
		cat <&0
	fi
}

command_exists() {
	command -v "$1" >/dev/null 2>&1
}

require_command() {
	if ! command_exists "$1"; then
		log "Required command '$1' not found"
		exit 1
	fi
}

normalize_sha() {
	value=$1
	case "$value" in
		SHA256=*|sha256=*)
		value=${value#*=}
		;;
	esac
	printf '%s' "$value" | tr 'A-Z' 'a-z'
}

sha256_of() {
	file=$1
	if command_exists shasum; then
		shasum -a 256 "$file" | awk '{print $1}'
	elif command_exists sha256sum; then
		sha256sum "$file" | awk '{print $1}'
	elif command_exists openssl; then
		openssl dgst -sha256 "$file" | awk '{print $NF}'
	else
		log "Need shasum, sha256sum, or openssl to verify checksums"
		exit 1
	fi
}

verify_sha256() {
	file=$1
	expected=$2
	actual=$(sha256_of "$file" | tr 'A-Z' 'a-z')
	if [ "$actual" != "$expected" ]; then
		log "Checksum mismatch for $file"
		log " expected: $expected"
		log "   actual: $actual"
		exit 1
	fi
}

version_matches() {
	required=$1
	installed=$2

	[ "$required" = "$installed" ] && return 0

	case "$installed" in
		"$required"|"$required".*)
		return 0
		;;
	esac

	wildcard=$(printf '%s' "$required" | tr 'X' '*' | tr 'x' '*')
	case "$installed" in
		$wildcard|$wildcard.*)
		return 0
		;;
	esac

	return 1
}

binary_has_version() {
	binary=$1
	required=$2

	[ -x "$binary" ] || return 1
	installed=$("$binary" -V 2>/dev/null) || return 1
	installed=$(printf '%s' "$installed" | tr -d '\r')
	[ -n "$installed" ] || return 1
	version_matches "$required" "$installed"
}

download_archive() {
	url=$1
	destination=$2

	tmp="${destination}.tmp.$$"
	if command_exists curl; then
		curl -L --fail --silent --show-error -o "$tmp" "$url" | output
	elif command_exists wget; then
		wget -q -O "$tmp" "$url" | output
	else
		log "Need curl or wget to download archive"
		rm -f "$tmp"
		exit 1
	fi
	mv "$tmp" "$destination"
}

extract_archive() {
	archive=$1
	workdir=$2
	require_command tar
	tar -xzf "$archive" -C "$workdir" | output
}

build_binary() {
	srcdir=$1
	make_bin=${MAKE:-make}
	require_command "$make_bin"
	(
		cd "$srcdir"
		if ! "$make_bin" | output; then
			log "Failed to build in $srcdir"
			exit 1
		fi
	)
}

WORKDIR=
URL=
RAW_SHA=
QUIET=

while [ $# -gt 0 ]; do
	case "$1" in
	-q)
		QUIET=true
		shift
		;;
	--workdir)
		[ $# -ge 2 ] || usage
		WORKDIR=$2
		shift 2
		;;
	--url)
		[ $# -ge 2 ] || usage
		URL=$2
		shift 2
		;;
	--sha256)
		[ $# -ge 2 ] || usage
		RAW_SHA=$2
		shift 2
		;;
	--help|-h)
		usage
		exit 0
		;;
	--)
		shift
		break
		;;
	--*)
		log "Unknown option: $1"
		usage
		;;
	*)
		break
		;;
	esac
done

[ $# -ge 2 ] || usage
PROGRAM=$1
VERSION=$2
shift 2
[ $# -eq 0 ] || usage

if [ -n "$WORKDIR" ]; then
	mkdir -p "$WORKDIR"
	WORKDIR=$(cd "$WORKDIR" && pwd)
else
	WORKDIR=$(pwd)
fi

ARCHIVE="$WORKDIR/${PROGRAM}-${VERSION}.tar.gz"
SRCDIR="$WORKDIR/${PROGRAM}-${VERSION}"
BINARY="$SRCDIR/$PROGRAM"

if command_exists "$PROGRAM"; then
	system_bin=$(command -v "$PROGRAM")
	if binary_has_version "$system_bin" "$VERSION"; then
		printf '%s\n' "$system_bin"
		exit 0
	fi
fi

if binary_has_version "$BINARY" "$VERSION"; then
	printf '%s\n' "$BINARY"
	exit 0
fi

if [ -z "$URL" ]; then
	log "Version $VERSION of $PROGRAM not found and no --url provided"
	exit 1
fi

if [ -n "$RAW_SHA" ]; then
	SHA256=$(normalize_sha "$RAW_SHA")
else
	SHA256=
	warn "No --sha256 provided; skipping checksum verification"
fi

if [ ! -f "$ARCHIVE" ]; then
	log "Downloading $PROGRAM from $URL"
	download_archive "$URL" "$ARCHIVE"
fi

if [ -n "$SHA256" ]; then
	verify_sha256 "$ARCHIVE" "$SHA256"
fi

if [ ! -x "$BINARY" ]; then
	log "Extracting into $SRCDIR"
	rm -rf "$SRCDIR"
	extract_archive "$ARCHIVE" "$WORKDIR"
	log "Building $PROGRAM"
	build_binary "$SRCDIR"
fi

if ! binary_has_version "$BINARY" "$VERSION"; then
	log "Built $PROGRAM does not match required version $VERSION"
	exit 1
fi

printf '%s\n' "$BINARY"
