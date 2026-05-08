project(GD32M531C_START LANGUAGES C CXX ASM)

add_library(GD32M531C_START OBJECT
    ${DRIVERS_DIR}/BSP/GD32M531C_START/gd32m531c_start.c
    )

target_include_directories(GD32M531C_START PUBLIC
    ${DRIVERS_DIR}/BSP/GD32M531C_START
    )

target_link_libraries(GD32M531C_START PUBLIC GD32M53x_standard_peripheral)
