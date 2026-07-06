# Zephyr application — top level. Keep this file boilerplate-only:
# it bootstraps the Zephyr build system and descends into src/.
cmake_minimum_required(VERSION 3.20.0)

# Pulls in the entire Zephyr build system and — crucially — creates the
# `app` library target that every other CMakeLists adds sources to.
# Must come BEFORE project().
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(esp32_starter)

add_subdirectory(src)
