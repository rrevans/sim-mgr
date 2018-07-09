#!/bin/bash
export DISPLAY=:0.0
xdotool search --name --sync --onlyvisible "obs" windowfocus
STATUS=$?
if [ $STATUS -eq 0 ]; then
	xdotool key ctrl+A
	STATUS=$?
	if [ $STATUS -eq 0 ]; then
		echo "Key Send for Start"
	fi
else
	echo "Failed to get window focus"
	exit $STATUS
fi
xdotool search --name --sync "Vet School Simulator" windowfocus
