#!/bin/sh

PATH=/usr/local/bin/:$PATH

# Check if yabai exists
if ! [ -x "$(command -v yabai)" ]; then
  echo "{\"error\":\"yabai binary not found\"}"
  exit 1
fi

SPACES_TERTIARY=$(yabai -m query --space3 --display 3)

echo $(cat <<-EOF
{
  "spaces_tertiary": $SPACES_TERTIARY
}
EOF
)
