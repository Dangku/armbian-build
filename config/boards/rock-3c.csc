# Rockchip RK3566 quad core 1/2GB RAM SoC WIFI/BT eMMC USB3 PCIe
BOARD_NAME="Rock 3C"
BOARDFAMILY="rk35xx"
BOARD_MAINTAINER=""
BOOTCONFIG="rock-3c-rk3566_defconfig"
KERNEL_TARGET="current,edge"
FULL_DESKTOP="yes"
BOOT_LOGO="desktop"
BOOT_FDT_FILE="rockchip/rk3566-rock-3c.dtb"
IMAGE_PARTITION_TABLE="gpt"
BOOT_SCENARIO="spl-blobs"
BOOTFS_TYPE="fat"