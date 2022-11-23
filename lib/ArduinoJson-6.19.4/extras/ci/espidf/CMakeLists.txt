@PACKAGE_INIT@

include("${CMAKE_CURRENT_LIST_DIR}/@PROJECT_NAME@Targets.cmake")
check_required_components("@PROJECT_NAME@")
# ArduinoJson - https://arduinojson.org
# Copyright © 2014-2022, Benoit BLANCHON
# MIT License

cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(example)
