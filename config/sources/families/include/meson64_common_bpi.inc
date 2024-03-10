#
# SPDX-License-Identifier: GPL-2.0
#
# Copyright (c) 2013-2023 Igor Pecovnik, igor@armbian.com
#
# This file is a part of the Armbian Build Framework
# https://github.com/armbian/build/
#
UBOOT_TARGET_MAP="${UBOOT_TARGET_MAP:-u-boot-dtb.img;;u-boot.bin.sd.bin:u-boot.bin u-boot-dtb.img}"
LINUXFAMILY=meson64
ARCH=arm64
SERIALCON=ttyS0
SRC_LOADADDR='LOADADDR=0x1080000'

function fetch_sources_tools__libreelec_amlogic_fip_blob_update() {
	fetch_from_repo "https://github.com/Dangku/amlogic-boot-fip" "amlogic-boot-fip" "branch:master"
}

# this family does not need it
ATF_COMPILE="no"

family_tweaks_bsp() {
	mkdir -p $destination/etc/udev/rules.d
	mkdir -p $destination/usr/local/bin

	mkdir -p "$destination"/etc/X11/xorg.conf.d
	case "${BOARD}" in
		"bpi-m5")
	cat <<-EOF > "$destination"/etc/X11/xorg.conf
	Section "Device"
	    Identifier  "DRM Graphics Acclerated"
	    
	    ## Use modesetting and glamor
	        Driver      "modesetting"
	        Option      "AccelMethod"    "glamor"     ### "glamor" to enable 3D acceleration, "none" to disable.
	        Option      "DRI"            "2"
	        Option      "Dri2Vsync"      "true"
	        Option      "TripleBuffer"   "True"
	    ## End glamor configuration
	    
	EndSection
       
	Section "Screen"
	    Identifier "Default Screen"
	    SubSection "Display"
	        Depth 24
	    EndSubSection
	EndSection
	EOF
        ;;
	esac
}
