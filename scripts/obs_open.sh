#!/bin/bash
#
# This file is part of the sim-mgr distribution (https://github.com/OpenVetSimDevelopers/sim-mgr).
# 
# Copyright (c) 2019 VetSim, Cornell University College of Veterinary Medicine Ithaca, NY
# 
# This program is free software: you can redistribute it and/or modify  
# it under the terms of the GNU General Public License as published by  
# the Free Software Foundation, version 3.
#
# This program is distributed in the hope that it will be useful, but 
# WITHOUT ANY WARRANTY; without even the implied warranty of 
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License 
# along with this program. If not, see <http://www.gnu.org/licenses/>.

read IPETH0  <<< $(pidof obs)
len=${#IPETH0}
if [ $len -eq 0 ]; then
	export DISPLAY=:0.0
	obs &

	xdotool search --name --sync --onlyvisible "OBS" windowfocus
	STATUS=$?
	if [ $STATUS -eq 0 ]; then
		xdotool getactivewindow windowminimize
		STATUS=$?
		if [ $STATUS -eq 0 ]; then
			echo "Failed window move"
		fi
	else
		echo "Failed to get window focus"
		exit $STATUS
	fi
	xdotool search --name "Vet School Simulator" windowactivate
fi