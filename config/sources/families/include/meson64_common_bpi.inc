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

# this family does not need it
ATF_COMPILE="no"

function fetch_sources_tools__meson_bpi_fip() {
	fetch_from_repo "https://github.com/Dangku/amlogic-boot-fip" "amlogic-boot-fip" "branch:master"
}

function add_host_dependencies__meson_bpi_hostdeb() {
	display_alert "$BOARD" "adding legacy build host deps" "info"

        declare -g EXTRA_BUILD_DEPS="${EXTRA_BUILD_DEPS} lib32stdc++6 zlib1g:i386 libc6-dev-i386 lib32ncurses5-dev"
}

function extension_finish_config__meson_bpi_kernelconfig() {
	display_alert "$BOARD" "Setting kernel config" "info"

        declare -g LINUXSOURCEDIR="linux-kernel-worktree/${KERNELDIR}"
}

function write_uboot_platform()
{
	display_alert "$BOARD" "Writing uboot to image" "info"

	dd if=$1/u-boot.bin of=$2 bs=512 seek=1 conv=fsync > /dev/null 2>&1
}

family_tweaks_bsp() {
	mkdir -p $destination/etc/udev/rules.d
	mkdir -p $destination/usr/local/bin

	mkdir -p "$destination"/etc/X11/xorg.conf.d
	case "${BOARD}" in
		"bpi-m5"|"bpi-m2s")
	cat <<-EOF > "$destination"/etc/X11/xorg.conf
	Section "Device"
	    Identifier  "Meson Fbdev"
	    
	    Driver "fbdev"
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

function custom_apt_repo__bpi_release_apt_list()
{
	display_alert "${BOARD}" "Restoring apt sources.list" "info"

	# restore apt sources.list
	if [[ "${DOWNLOAD_MIRROR}" == "china" && "${BUILD_TYPE}" == "release" ]]; then
		DEBIAN_MIRROR='deb.debian.org/debian'
		DEBIAN_SECURTY='deb.debian.org/debian-security/'
		UBUNTU_MIRROR='ports.ubuntu.com/ubuntu-ports'
		ARMBIAN_MIRROR="apt.armbian.com"

		case ${RELEASE} in
			"buster")
				cat <<- EOF > "${SDCARD}"/etc/apt/sources.list
					deb http://${DEBIAN_MIRROR} ${RELEASE} main contrib non-free
					#deb-src http://${DEBIAN_MIRROR} ${RELEASE} main contrib non-free

					deb http://${DEBIAN_MIRROR} ${RELEASE}-updates main contrib non-free
					#deb-src http://${DEBIAN_MIRROR} ${RELEASE}-updates main contrib non-free

					deb http://${DEBIAN_MIRROR} ${RELEASE}-backports main contrib non-free
					#deb-src http://${DEBIAN_MIRROR} ${RELEASE}-backports main contrib non-free

					deb http://${DEBIAN_SECURTY} ${RELEASE}/updates main contrib non-free
					#deb-src http://${DEBIAN_SECURTY} ${RELEASE}/updates main contrib non-free
				EOF
				;;

			"bullseye")
				cat <<- EOF > "${SDCARD}"/etc/apt/sources.list
					deb http://${DEBIAN_MIRROR} ${RELEASE} main contrib non-free
					#deb-src http://${DEBIAN_MIRROR} ${RELEASE} main contrib non-free

					deb http://${DEBIAN_MIRROR} ${RELEASE}-updates main contrib non-free
					#deb-src http://${DEBIAN_MIRROR} ${RELEASE}-updates main contrib non-free

					deb http://${DEBIAN_MIRROR} ${RELEASE}-backports main contrib non-free
					#deb-src http://${DEBIAN_MIRROR} ${RELEASE}-backports main contrib non-free

					deb http://${DEBIAN_SECURTY} ${RELEASE}-security main contrib non-free
					#deb-src http://${DEBIAN_SECURTY} ${RELEASE}-security main contrib non-free
				EOF
				;;

			"bookworm")
				cat <<- EOF > "${SDCARD}"/etc/apt/sources.list
					deb http://${DEBIAN_MIRROR} ${RELEASE} main contrib non-free non-free-firmware
					#deb-src http://${DEBIAN_MIRROR} ${RELEASE} main contrib non-free non-free-firmware

					deb http://${DEBIAN_MIRROR} ${RELEASE}-updates main contrib non-free non-free-firmware
					#deb-src http://${DEBIAN_MIRROR} ${RELEASE}-updates main contrib non-free non-free-firmware

					deb http://${DEBIAN_MIRROR} ${RELEASE}-backports main contrib non-free non-free-firmware
					#deb-src http://${DEBIAN_MIRROR} ${RELEASE}-backports main contrib non-free non-free-firmware

					deb http://${DEBIAN_SECURTY} ${RELEASE}-security main contrib non-free non-free-firmware
					#deb-src http://${DEBIAN_SECURTY} ${RELEASE}-security main contrib non-free non-free-firmware
				EOF
				;;

			"focal"|"jammy")
				cat <<- EOF > "${SDCARD}"/etc/apt/sources.list
					deb http://${UBUNTU_MIRROR} ${RELEASE} main restricted universe multiverse
					#deb-src http://${UBUNTU_MIRROR} ${RELEASE} main restricted universe multiverse
					deb http://${UBUNTU_MIRROR} ${RELEASE}-security main restricted universe multiverse
					#deb-src http://${UBUNTU_MIRROR} ${RELEASE}-security main restricted universe multiverse
					deb http://${UBUNTU_MIRROR} ${RELEASE}-updates main restricted universe multiverse
					#deb-src http://${UBUNTU_MIRROR} ${RELEASE}-updates main restricted universe multiverse
					deb http://${UBUNTU_MIRROR} ${RELEASE}-backports main restricted universe multiverse
					#deb-src http://${UBUNTU_MIRROR} ${RELEASE}-backports main restricted universe multiverse
				EOF
				;;
		esac

		if [[ "${SKIP_ARMBIAN_REPO}" != "yes" ]]; then
			# no apt mirror for buster and focal
			if [[ "${RELEASE}" != "buster" && "${RELEASE}" != "focal" ]]; then
				cat <<- EOF > "${SDCARD}"/etc/apt/sources.list.d/armbian.list
					deb http://${ARMBIAN_MIRROR} $RELEASE main ${RELEASE}-utils ${RELEASE}-desktop
				EOF
			fi
		fi

	fi
}
