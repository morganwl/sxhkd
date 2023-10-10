#!/bin/bash

# temp file for comparing output
TMPOUTPUT=$(mktemp)

# start virtualized X server
Xvfb :99 &
XVFB_PID=$!
export DISPLAY=:99

# poll until server is ready
while ! xdpyinfo > /dev/null 2>&1; do
    sleep 0.01
done

# start sxhkd
./sxhkd -c "test/functional/hk_groups_rc" > "$TMPOUTPUT" &
SXHKD_PID=$!
sleep 0.05

xdotool key a
xdotool key b
xdotool key c
xdotool key d

kill $SXHKD_PID
kill $XVFB_PID

expected=$'Caught a\nCaught b\nCaught c\nCaught d'
result="$(<$TMPOUTPUT)"
printf "$0:"
if [ "$result" != "$expected" ]; then
    printf "FAIL\n"
    printf "Expected:\n$expected\n"
    printf "Got:\n$result\n"
else
    echo "PASS"
    echo "OK"
fi
