# SPDX-License-Identifier: Apache-2.0

zephyr_sources_ifdef(CONFIG_I2C_EEPROM_SLAVE	eeprom_slave.c)
zephyr_sources_ifdef(CONFIG_I2C_IPMB_SLAVE	ipmb_slave.c)
