#!/bin/sh

if [ -z "$1" ]; then
  echo "Usage: $0 filename"
  exit 1
fi

input="$1"
output="$input"

# Conditions for conditional branches
conds="eq|ne|cs|cc|mi|pl|vs|vc|hi|ls|ge|lt|gt|le|al|nv|hs|lo"

tmp=$(mktemp)

sed -E "
  s/^([[:space:]]*)([0-9][a-zA-Z0-9]*:)/\1.\2/
  s/\b(b(\.?($conds))?[[:space:]]+)([0-9][a-zA-Z0-9]*)/\1.\4/
  s/\b(bl)[[:space:]]+([0-9][a-zA-Z0-9]*)/\1 .\2/
  s/\b(cbz|cbnz)[[:space:]]+([^,]+,[[:space:]]*)([0-9][a-zA-Z0-9]*)/\1 \2.\3/
  s/\b(tbz)[[:space:]]+([^,]+,[[:space:]]*[^,]+,[[:space:]]*)([0-9][a-zA-Z0-9]*)/\1 \2.\3/
" "$input" \
  | grep -vE '^\s*($|#|//)' > "$tmp"

mv "$tmp" "$output"
echo "Cleaned file saved as $output"
