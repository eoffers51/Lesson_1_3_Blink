# Host tests: run the logic of the components on the PC (no board needed).
# Uses Unity, the same test framework ESP-IDF uses on the chip.
cmake_minimum_required(VERSION 3.16)
project(host_test C)

# Where ESP-IDF is (only Unity is taken from it): pass -DIDF_PATH=... or set the IDF_PATH variable
set(IDF_PATH "$ENV{IDF_PATH}" CACHE PATH "ESP-IDF directory")
if(NOT EXISTS "${IDF_PATH}/components/unity")
    message(FATAL_ERROR "ESP-IDF not found: pass -DIDF_PATH=C:/esp/v6.0.2/esp-idf")
endif()

set(UNITY_DIR "${IDF_PATH}/components/unity/unity/src")
set(COMPONENTS_DIR "${CMAKE_CURRENT_LIST_DIR}/../components")

add_executable(host_test
    test_main.c
    test_siren.c
    test_button.c
    fakes/fake_gpio.c
    "${UNITY_DIR}/unity.c"
    "${COMPONENTS_DIR}/siren/siren.c"
    "${COMPONENTS_DIR}/button/button.c")

# fakes/ comes first so button.c gets the fake driver/gpio.h and esp_err.h
target_include_directories(host_test PRIVATE
    fakes
    "${UNITY_DIR}"
    "${COMPONENTS_DIR}/siren/include"
    "${COMPONENTS_DIR}/button/include")

target_compile_options(host_test PRIVATE -Wall -Wextra -Werror)

enable_testing()
add_test(NAME host_test COMMAND host_test)
