project(GD32M53x_standard_peripheral LANGUAGES C CXX ASM)

# Comment-out unused source files.
add_library(GD32M53x_standard_peripheral OBJECT
	${DRIVERS_DIR}/GD32M53x_standard_peripheral/Source/gd32m53x_adc.c
    ${DRIVERS_DIR}/GD32M53x_standard_peripheral/Source/gd32m53x_can.c
    ${DRIVERS_DIR}/GD32M53x_standard_peripheral/Source/gd32m53x_cfmu.c
    ${DRIVERS_DIR}/GD32M53x_standard_peripheral/Source/gd32m53x_cmp.c
    ${DRIVERS_DIR}/GD32M53x_standard_peripheral/Source/gd32m53x_cptimer.c
    ${DRIVERS_DIR}/GD32M53x_standard_peripheral/Source/gd32m53x_cptimerw.c
    ${DRIVERS_DIR}/GD32M53x_standard_peripheral/Source/gd32m53x_crc.c
    ${DRIVERS_DIR}/GD32M53x_standard_peripheral/Source/gd32m53x_dac.c
    ${DRIVERS_DIR}/GD32M53x_standard_peripheral/Source/gd32m53x_dbg.c
    ${DRIVERS_DIR}/GD32M53x_standard_peripheral/Source/gd32m53x_dma.c
    ${DRIVERS_DIR}/GD32M53x_standard_peripheral/Source/gd32m53x_evic.c
    ${DRIVERS_DIR}/GD32M53x_standard_peripheral/Source/gd32m53x_exti.c
    ${DRIVERS_DIR}/GD32M53x_standard_peripheral/Source/gd32m53x_fmc.c
    ${DRIVERS_DIR}/GD32M53x_standard_peripheral/Source/gd32m53x_fwdgt.c
    ${DRIVERS_DIR}/GD32M53x_standard_peripheral/Source/gd32m53x_gpio.c
    ${DRIVERS_DIR}/GD32M53x_standard_peripheral/Source/gd32m53x_gptimer.c
    ${DRIVERS_DIR}/GD32M53x_standard_peripheral/Source/gd32m53x_gtoc.c
    ${DRIVERS_DIR}/GD32M53x_standard_peripheral/Source/gd32m53x_i2c.c
    ${DRIVERS_DIR}/GD32M53x_standard_peripheral/Source/gd32m53x_misc.c
    ${DRIVERS_DIR}/GD32M53x_standard_peripheral/Source/gd32m53x_pmu.c
    ${DRIVERS_DIR}/GD32M53x_standard_peripheral/Source/gd32m53x_poc.c
    ${DRIVERS_DIR}/GD32M53x_standard_peripheral/Source/gd32m53x_rcu.c
    ${DRIVERS_DIR}/GD32M53x_standard_peripheral/Source/gd32m53x_spi.c
    ${DRIVERS_DIR}/GD32M53x_standard_peripheral/Source/gd32m53x_svpwm.c
    ${DRIVERS_DIR}/GD32M53x_standard_peripheral/Source/gd32m53x_syscfg.c
    ${DRIVERS_DIR}/GD32M53x_standard_peripheral/Source/gd32m53x_timer.c
    ${DRIVERS_DIR}/GD32M53x_standard_peripheral/Source/gd32m53x_tmu.c
    ${DRIVERS_DIR}/GD32M53x_standard_peripheral/Source/gd32m53x_uart.c
    ${DRIVERS_DIR}/GD32M53x_standard_peripheral/Source/gd32m53x_wwdgt.c
    )

target_include_directories(GD32M53x_standard_peripheral PUBLIC
    ${DRIVERS_DIR}/GD32M53x_standard_peripheral/Include
    )

# CMSIS header only library is linked.
target_link_libraries(GD32M53x_standard_peripheral PUBLIC CMSIS)
