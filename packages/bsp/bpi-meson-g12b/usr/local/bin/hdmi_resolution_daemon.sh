#!/bin/bash

PIPE="/tmp/hdmi_resolution_pipe"
HDMI_HANDLER="/usr/local/bin/hdmi.sh"
INIT_RES_CONFIG="/var/local/hdmi_resolution"

# Create a pipe for communication with clients
if [[ ! -p "${PIPE}" ]]; then
	mkfifo "${PIPE}"
fi

if [[ ! -d ${INIT_RES_CONFIG} ]]; then
	mkdir ${INIT_RES_CONFIG}
fi

chmod 777 ${PIPE}

check_resolution() {
	resolution="$1"

	case $resolution in
		*p60hz|*p30hz)
			return 0
			;;
		*)
			return -1
			;;
	esac
}

hdmi_resolution_handler() {

	resolution=`echo $1 | awk -F "_" '{print $3}'`

	if ! check_resolution $resolution; then
		return -1
	fi

	echo "$resolution" > ${INIT_RES_CONFIG}/resolution

	sudo systemctl stop lightdm.service
	$HDMI_HANDLER $resolution
	sudo systemctl start lightdm.service

}

res=`cat ${INIT_RES_CONFIG}/resolution`
INIT_EVENT="change_resolution_$res"
hdmi_resolution_handler $INIT_EVENT

while true
do
	if read EVENT <> "${PIPE}"; then
		case $EVENT in
			change_resolution_*)
				hdmi_resolution_handler $EVENT
				;;
		esac
	fi
done
