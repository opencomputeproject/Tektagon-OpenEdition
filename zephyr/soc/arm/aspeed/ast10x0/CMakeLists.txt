# Copyright (c) 2021 ASPEED Technology Inc.
#
# SPDX-License-Identifier: Apache-2.0
#

zephyr_sources(soc.c)
zephyr_sources_ifdef(
  CONFIG_CPU_HAS_CUSTOM_FIXED_SOC_MPU_REGIONS
  mpu_regions.c
  )
zephyr_include_directories(${ZEPHYR_BASE}/soc/arm/common/cortex_m)
zephyr_linker_sources(ROM_START SORT_KEY 0x1sboot sboot.ld)
zephyr_linker_sources(RAM_SECTIONS nocache.ld)

set_property(GLOBAL APPEND PROPERTY extra_post_build_commands
  COMMAND sh ${SOC_DIR}/${ARCH}/${SOC_FAMILY}/${SOC_SERIES}/tools/gen_uart_booting_image.sh
  ${PROJECT_BINARY_DIR}/${CONFIG_KERNEL_BIN_NAME}.bin
  ${PROJECT_BINARY_DIR}/uart_${CONFIG_KERNEL_BIN_NAME}.bin
)
