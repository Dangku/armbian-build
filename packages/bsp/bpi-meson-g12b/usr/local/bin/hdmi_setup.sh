#!/bin/bash

# Only for desktop
if ! sudo which lightdm; then
	exit
fi

source /etc/os-release

PIPE="/tmp/hdmi_resolution_pipe"
resolutions=()
tempfile=$(mktemp)

# Create a pipe for communication with clients
if [[ ! -p "${PIPE}" ]]; then
	mkfifo "${PIPE}"
fi

sudo chmod 777 ${PIPE}

# Established timings supported
case "${VERSION_CODENAME}" in
	"focal"|"buster"|"bullseye"|"jammy"|"bookworm")
		edid-decode < /sys/class/amhdmitx/amhdmitx0/rawedid | grep "E:" > $tempfile
		while read line
		do
			resolutions+=(`echo $line | grep -v i | awk -F ":" '{print $2}' | awk '{print $1}'`)
		done < $tempfile

		# Standard timings supported
		edid-decode < /sys/class/amhdmitx/amhdmitx0/rawedid | grep "S:" > $tempfile
		while read line
		do
			resolutions+=(`echo $line | grep -v i | awk -F ":" '{print $2}' | awk '{print $1}'`)
		done < $tempfile

		# CEA modes
		edid-decode < /sys/class/amhdmitx/amhdmitx0/rawedid | grep "    VIC" | grep -v HDMI > $tempfile
		while read line
		do
			resolutions+=(`echo $line | grep -v i | awk '{print $3}'`)
		done < $tempfile
		;;
esac

##################
resolutions=(`echo ${resolutions[@]} | tr 'A-Z' 'a-z' | sed s/@/p/g`)

echo ${resolutions[@]} | tr ' ' '\n' > $tempfile

resolutions=(`sort -n -k 1 -k 2 -t x $tempfile | uniq | sed -e s/720x480.*/480p60hz/g -e s/720x576.*/576p60hz/g -e s/1280x720.*/720p60hz/g -e s/1920x1080.*/1080p60hz/g -e s/3840x2160.*/2160p60hz/g | uniq | tr '\n' ' '`)

current_resolution=`cat /sys/class/display/mode`

res=$(yad --entry "${resolutions[@]}" \
	--entry-text=$current_resolution \
	--entry-label 'Resolution' \
	--title 'HDMI Resolution Setting' \
	--width=600 \
	--height=50 \
	--window-icon=/usr/share/icons/armbian/hdmi_resolution.png)

if [ -z "$res" ] || [ "$res" == "$current_resolution" ]; then
	exit
fi

zenity --question --text 'Change the resolution will logout your system, please save your files!' \
	--title 'Warning' \
	--window-icon /usr/share/icons/armbian/warning.png \
	--width=600 \
	--height=50

if [ $? -ne 0 ]; then
	exit
fi

EVENT="change_resolution_$res"

echo $EVENT > $PIPE

exit
