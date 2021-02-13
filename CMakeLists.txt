# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)
SET(BOARD stm32_min_dev_blue)
SET(NO_BUILD_TYPE_WARNING 1)

SET(CONFIG_USB_RAZER_TYPE,0)
SET(CONF_FILE, prj.conf)

SET(ZEPHYR_BASE /home/benjamin/private/stm32/zephyrproject/zephyr)
list(APPEND CMAKE_PREFIX_PATH ${ZEPHYR_BASE}/share/zephyr-package/cmake)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(hid-mouse)


FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources})
