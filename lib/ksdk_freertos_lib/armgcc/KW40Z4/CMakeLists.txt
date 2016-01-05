INCLUDE(CMakeForceCompiler)

# CROSS COMPILER SETTING
SET(CMAKE_SYSTEM_NAME Generic)
CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

# THE VERSION NUMBER
SET (Tutorial_VERSION_MAJOR 1)
SET (Tutorial_VERSION_MINOR 0)

# ENABLE ASM
ENABLE_LANGUAGE(ASM)

SET(CMAKE_STATIC_LIBRARY_PREFIX)
SET(CMAKE_STATIC_LIBRARY_SUFFIX)

SET(CMAKE_EXECUTABLE_LIBRARY_PREFIX)
SET(CMAKE_EXECUTABLE_LIBRARY_SUFFIX)

 
# CURRENT DIRECTORY
SET(ProjDirPath ${CMAKE_CURRENT_SOURCE_DIR})

# DEBUG ASM FLAGS
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG}  -g  -mcpu=cortex-m0plus  -mthumb  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -mapcs  -std=gnu99 ")

# DEBUG C FLAGS
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g  -O0  -mcpu=cortex-m0plus  -mthumb  -MMD  -MP  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -mapcs  -std=gnu99")

# RELEASE ASM FLAGS
SET(CMAKE_ASM_FLAGS_RELEASE "${CMAKE_ASM_FLAGS_RELEASE}  -mcpu=cortex-m0plus  -mthumb  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -mapcs  -std=gnu99 ")

# RELEASE C FLAGS
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Os  -mcpu=cortex-m0plus  -mthumb  -MMD  -MP  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -mapcs  -std=gnu99")

# ASM MACRO
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG}  -DDEBUG")

# C MACRO
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DDEBUG")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DCPU_MKW40Z160VHT4")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DFSL_RTOS_FREE_RTOS")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DUSB_KW40Z")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DDONGLE")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DNDEBUG")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DCPU_MKW40Z160VHT4")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DFSL_RTOS_FREE_RTOS")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DUSB_KW40Z")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DDONGLE")

# CXX MACRO

# INCLUDE_DIRECTORIES
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../rtos/FreeRTOS/port/gcc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../rtos/FreeRTOS/config/KW40Z4/gcc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../rtos/FreeRTOS/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../rtos/FreeRTOS/src)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/CMSIS/Include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/devices)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/devices/MKW40Z4/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/devices/MKW40Z4/startup)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/hal/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/drivers/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/system/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/osa/inc)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../rtos/FreeRTOS/port/gcc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../rtos/FreeRTOS/config/KW40Z4/gcc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../rtos/FreeRTOS/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../rtos/FreeRTOS/src)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/CMSIS/Include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/devices)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/devices/MKW40Z4/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/devices/MKW40Z4/startup)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/hal/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/drivers/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/system/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/osa/inc)
ENDIF()

# ADD_LIBRARY
ADD_LIBRARY(KsdkFreertosLib STATIC
    "${ProjDirPath}/../../../../platform/osa/inc/fsl_os_abstraction.h"
    "${ProjDirPath}/../../../../platform/osa/inc/fsl_os_abstraction_free_rtos.h"
    "${ProjDirPath}/../../../../platform/osa/src/fsl_os_abstraction_free_rtos.c"
    "${ProjDirPath}/../../../../platform/system/src/clock/fsl_clock_manager.c"
    "${ProjDirPath}/../../../../platform/system/inc/fsl_clock_manager.h"
    "${ProjDirPath}/../../../../platform/system/src/clock/fsl_clock_manager_common.c"
    "${ProjDirPath}/../../../../platform/system/src/clock/MKW40Z4/fsl_clock_MKW40Z4.c"
    "${ProjDirPath}/../../../../platform/system/src/clock/MKW40Z4/fsl_clock_MKW40Z4.h"
    "${ProjDirPath}/../../../../platform/system/src/interrupt/fsl_interrupt_manager.c"
    "${ProjDirPath}/../../../../platform/system/inc/fsl_interrupt_manager.h"
    "${ProjDirPath}/../../../../platform/system/src/hwtimer/fsl_hwtimer.c"
    "${ProjDirPath}/../../../../platform/system/inc/fsl_hwtimer.h"
    "${ProjDirPath}/../../../../platform/system/src/hwtimer/fsl_hwtimer_pit.c"
    "${ProjDirPath}/../../../../platform/system/inc/fsl_hwtimer_pit.h"
    "${ProjDirPath}/../../../../platform/system/src/hwtimer/fsl_hwtimer_systick.c"
    "${ProjDirPath}/../../../../platform/system/inc/fsl_hwtimer_systick.h"
    "${ProjDirPath}/../../../../platform/system/src/power/fsl_power_manager.c"
    "${ProjDirPath}/../../../../platform/system/inc/fsl_power_manager.h"
    "${ProjDirPath}/../../../../platform/system/src/power/fsl_power_manager_common.h"
    "${ProjDirPath}/../../../../platform/hal/src/adc16/fsl_adc16_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_adc16_hal.h"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_adc16_driver.h"
    "${ProjDirPath}/../../../../platform/drivers/src/adc16/fsl_adc16_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/src/adc16/fsl_adc16_common.c"
    "${ProjDirPath}/../../../../platform/hal/src/cmp/fsl_cmp_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_cmp_hal.h"
    "${ProjDirPath}/../../../../platform/drivers/src/cmp/fsl_cmp_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_cmp_driver.h"
    "${ProjDirPath}/../../../../platform/drivers/src/cmp/fsl_cmp_common.c"
    "${ProjDirPath}/../../../../platform/hal/src/cmt/fsl_cmt_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_cmt_hal.h"
    "${ProjDirPath}/../../../../platform/drivers/src/cmt/fsl_cmt_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_cmt_driver.h"
    "${ProjDirPath}/../../../../platform/drivers/src/cmt/fsl_cmt_common.c"
    "${ProjDirPath}/../../../../platform/hal/src/cop/fsl_cop_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_cop_hal.h"
    "${ProjDirPath}/../../../../platform/drivers/src/cop/fsl_cop_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_cop_driver.h"
    "${ProjDirPath}/../../../../platform/drivers/src/cop/fsl_cop_common.c"
    "${ProjDirPath}/../../../../platform/hal/src/dac/fsl_dac_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_dac_hal.h"
    "${ProjDirPath}/../../../../platform/drivers/src/dac/fsl_dac_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_dac_driver.h"
    "${ProjDirPath}/../../../../platform/drivers/src/dac/fsl_dac_common.c"
    "${ProjDirPath}/../../../../platform/hal/src/dma/fsl_dma_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_dma_hal.h"
    "${ProjDirPath}/../../../../platform/drivers/src/dma/fsl_dma_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_dma_driver.h"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_dma_request.h"
    "${ProjDirPath}/../../../../platform/drivers/src/dma/fsl_dma_common.c"
    "${ProjDirPath}/../../../../platform/hal/src/dmamux/fsl_dmamux_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_dmamux_hal.h"
    "${ProjDirPath}/../../../../platform/hal/src/dspi/fsl_dspi_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_dspi_hal.h"
    "${ProjDirPath}/../../../../platform/drivers/src/dspi/fsl_dspi_shared_function.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_dspi_shared_function.h"
    "${ProjDirPath}/../../../../platform/drivers/src/dspi/fsl_dspi_common.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_dspi_master_driver.h"
    "${ProjDirPath}/../../../../platform/drivers/src/dspi/fsl_dspi_master_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_dspi_slave_driver.h"
    "${ProjDirPath}/../../../../platform/drivers/src/dspi/fsl_dspi_slave_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_dspi_dma_slave_driver.h"
    "${ProjDirPath}/../../../../platform/drivers/src/dspi/fsl_dspi_dma_slave_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_dspi_dma_master_driver.h"
    "${ProjDirPath}/../../../../platform/drivers/src/dspi/fsl_dspi_dma_master_driver.c"
    "${ProjDirPath}/../../../../platform/hal/src/gpio/fsl_gpio_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_gpio_hal.h"
    "${ProjDirPath}/../../../../platform/drivers/src/gpio/fsl_gpio_common.c"
    "${ProjDirPath}/../../../../platform/drivers/src/gpio/fsl_gpio_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_gpio_driver.h"
    "${ProjDirPath}/../../../../platform/hal/src/i2c/fsl_i2c_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_i2c_hal.h"
    "${ProjDirPath}/../../../../platform/drivers/src/i2c/fsl_i2c_shared_function.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_i2c_shared_function.h"
    "${ProjDirPath}/../../../../platform/drivers/src/i2c/fsl_i2c_common.c"
    "${ProjDirPath}/../../../../platform/drivers/src/i2c/fsl_i2c_master_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_i2c_master_driver.h"
    "${ProjDirPath}/../../../../platform/drivers/src/i2c/fsl_i2c_slave_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_i2c_slave_driver.h"
    "${ProjDirPath}/../../../../platform/hal/src/llwu/fsl_llwu_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_llwu_hal.h"
    "${ProjDirPath}/../../../../platform/hal/src/lptmr/fsl_lptmr_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_lptmr_hal.h"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_lptmr_driver.h"
    "${ProjDirPath}/../../../../platform/drivers/src/lptmr/fsl_lptmr_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/src/lptmr/fsl_lptmr_common.c"
    "${ProjDirPath}/../../../../platform/hal/src/ltc/fsl_ltc_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_ltc_hal.h"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_ltc_driver.h"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_ltc_driver_aes.h"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_ltc_driver_hash.h"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_ltc_driver_prv.h"
    "${ProjDirPath}/../../../../platform/drivers/src/ltc/fsl_ltc_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/src/ltc/fsl_ltc_driver_aes.c"
    "${ProjDirPath}/../../../../platform/drivers/src/ltc/fsl_ltc_driver_aes_ccm.c"
    "${ProjDirPath}/../../../../platform/drivers/src/ltc/fsl_ltc_driver_hash.c"
    "${ProjDirPath}/../../../../platform/drivers/src/ltc/fsl_ltc_common.c"
    "${ProjDirPath}/../../../../platform/hal/src/trng/fsl_trng_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_trng_hal.h"
    "${ProjDirPath}/../../../../platform/drivers/src/trng/fsl_trng_common.c"
    "${ProjDirPath}/../../../../platform/drivers/src/trng/fsl_trng_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_trng_driver.h"
    "${ProjDirPath}/../../../../platform/hal/src/lpuart/fsl_lpuart_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_lpuart_hal.h"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_lpuart_driver.h"
    "${ProjDirPath}/../../../../platform/drivers/src/lpuart/fsl_lpuart_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/src/lpuart/fsl_lpuart_common.c"
    "${ProjDirPath}/../../../../platform/drivers/src/lpuart/fsl_lpuart_dma_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_lpuart_dma_driver.h"
    "${ProjDirPath}/../../../../platform/hal/src/mcg/fsl_mcg_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_mcg_hal.h"
    "${ProjDirPath}/../../../../platform/hal/src/mcg/fsl_mcg_hal_modes.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_mcg_hal_modes.h"
    "${ProjDirPath}/../../../../platform/hal/src/pit/fsl_pit_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_pit_hal.h"
    "${ProjDirPath}/../../../../platform/drivers/src/pit/fsl_pit_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_pit_driver.h"
    "${ProjDirPath}/../../../../platform/drivers/src/pit/fsl_pit_common.c"
    "${ProjDirPath}/../../../../platform/hal/src/port/fsl_port_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_port_hal.h"
    "${ProjDirPath}/../../../../platform/hal/src/rtc/fsl_rtc_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_rtc_hal.h"
    "${ProjDirPath}/../../../../platform/drivers/src/rtc/fsl_rtc_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_rtc_driver.h"
    "${ProjDirPath}/../../../../platform/drivers/src/rtc/fsl_rtc_common.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_sim_hal.h"
    "${ProjDirPath}/../../../../platform/hal/src/sim/MKW40Z4/fsl_sim_hal_MKW40Z4.c"
    "${ProjDirPath}/../../../../platform/hal/src/sim/MKW40Z4/fsl_sim_hal_MKW40Z4.h"
    "${ProjDirPath}/../../../../platform/hal/src/smc/fsl_smc_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_smc_hal.h"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_pmc_hal.h"
    "${ProjDirPath}/../../../../platform/hal/src/rcm/fsl_rcm_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_rcm_hal.h"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_tpm_hal.h"
    "${ProjDirPath}/../../../../platform/hal/src/tpm/fsl_tpm_hal.c"
    "${ProjDirPath}/../../../../platform/drivers/src/tpm/fsl_tpm_common.c"
    "${ProjDirPath}/../../../../platform/drivers/src/tpm/fsl_tpm_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_tpm_driver.h"
    "${ProjDirPath}/../../../../platform/hal/src/tsi/fsl_tsi_hal.c"
    "${ProjDirPath}/../../../../platform/hal/src/tsi/fsl_tsi_v4_hal_specific.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_tsi_hal.h"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_tsi_v4_hal_specific.h"
    "${ProjDirPath}/../../../../platform/drivers/src/tsi/fsl_tsi_common.c"
    "${ProjDirPath}/../../../../platform/drivers/src/tsi/fsl_tsi_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/src/tsi/fsl_tsi_v4_driver_specific.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_tsi_driver.h"
)

SET_TARGET_PROPERTIES(KsdkFreertosLib PROPERTIES OUTPUT_NAME "libksdk_platform_freertos.a")
