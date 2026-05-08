project(GD32M531R_EVAL LANGUAGES C CXX ASM)

add_library(GD32M531R_EVAL OBJECT
    ${DRIVERS_DIR}/BSP/GD32M531R_EVAL/gd32m531r_eval.c
    )

target_include_directories(GD32M531R_EVAL PUBLIC
    ${DRIVERS_DIR}/BSP/GD32M531R_EVAL
    )

target_link_libraries(GD32M531R_EVAL PUBLIC GD32M53x_standard_peripheral)
