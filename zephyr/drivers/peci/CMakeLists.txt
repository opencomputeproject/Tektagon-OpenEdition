# SPDX-License-Identifier: Apache-2.0

zephyr_library()

zephyr_library_sources_ifdef(CONFIG_PECI_SHELL  peci_shell.c)
zephyr_library_sources_ifdef(CONFIG_PECI_ASPEED	peci_aspeed.c)
zephyr_library_sources_ifdef(CONFIG_PECI_XEC	peci_mchp_xec.c)
zephyr_library_sources_ifdef(CONFIG_USERSPACE   peci_handlers.c)
