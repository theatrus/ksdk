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
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG}  -g  -mcpu=cortex-m4  -mfloat-abi=soft  -mthumb  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99 ")

# DEBUG C FLAGS
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g  -mcpu=cortex-m4  -mfloat-abi=soft  -mthumb  -MMD  -MP  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# RELEASE ASM FLAGS
SET(CMAKE_ASM_FLAGS_RELEASE "${CMAKE_ASM_FLAGS_RELEASE}  -mcpu=cortex-m4  -mfloat-abi=soft  -mthumb  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99 ")

# RELEASE C FLAGS
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -mcpu=cortex-m4  -mfloat-abi=soft  -mthumb  -MMD  -MP  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# ASM MACRO
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG}  -DDEBUG")

# C MACRO
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DDEBUG")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DCPU_MK60DN512VMD10")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DFSL_RTOS_FREE_RTOS")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DNDEBUG")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DCPU_MK60DN512VMD10")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DFSL_RTOS_FREE_RTOS")

# CXX MACRO

# INCLUDE_DIRECTORIES
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../rtos/FreeRTOS/port/gcc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../rtos/FreeRTOS/config/K60D10/gcc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../rtos/FreeRTOS/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../rtos/FreeRTOS/src)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/CMSIS/Include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/devices)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/devices/MK60D10/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/devices/MK60D10/startup)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/hal/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/drivers/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/system/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/osa/inc)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../rtos/FreeRTOS/port/gcc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../rtos/FreeRTOS/config/K60D10/gcc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../rtos/FreeRTOS/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../rtos/FreeRTOS/src)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/CMSIS/Include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/devices)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/devices/MK60D10/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/devices/MK60D10/startup)
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
    "${ProjDirPath}/../../../../platform/system/src/clock/MK60D10/fsl_clock_MK60D10.c"
    "${ProjDirPath}/../../../../platform/system/src/clock/MK60D10/fsl_clock_MK60D10.h"
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
    "${ProjDirPath}/../../../../platform/hal/src/flexbus/fsl_flexbus_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_flexbus_hal.h"
    "${ProjDirPath}/../../../../platform/drivers/src/flexbus/fsl_flexbus_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_flexbus_driver.h"
    "${ProjDirPath}/../../../../platform/drivers/src/flexbus/fsl_flexbus_common.c"
    "${ProjDirPath}/../../../../platform/hal/src/flexcan/fsl_flexcan_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_flexcan_hal.h"
    "${ProjDirPath}/../../../../platform/drivers/src/flexcan/fsl_flexcan_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_flexcan_driver.h"
    "${ProjDirPath}/../../../../platform/drivers/src/flexcan/fsl_flexcan_common.c"
    "${ProjDirPath}/../../../../platform/hal/src/cmp/fsl_cmp_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_cmp_hal.h"
    "${ProjDirPath}/../../../../platform/drivers/src/cmp/fsl_cmp_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_cmp_driver.h"
    "${ProjDirPath}/../../../../platform/drivers/src/cmp/fsl_cmp_common.c"
    "${ProjDirPath}/../../../../platform/hal/src/crc/fsl_crc_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_crc_hal.h"
    "${ProjDirPath}/../../../../platform/drivers/src/crc/fsl_crc_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_crc_driver.h"
    "${ProjDirPath}/../../../../platform/drivers/src/crc/fsl_crc_common.c"
    "${ProjDirPath}/../../../../platform/hal/src/dac/fsl_dac_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_dac_hal.h"
    "${ProjDirPath}/../../../../platform/drivers/src/dac/fsl_dac_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_dac_driver.h"
    "${ProjDirPath}/../../../../platform/drivers/src/dac/fsl_dac_common.c"
    "${ProjDirPath}/../../../../platform/hal/src/dmamux/fsl_dmamux_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_dmamux_hal.h"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_edma_hal.h"
    "${ProjDirPath}/../../../../platform/hal/src/edma/fsl_edma_hal.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_edma_driver.h"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_edma_request.h"
    "${ProjDirPath}/../../../../platform/drivers/src/edma/fsl_edma_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/src/edma/fsl_edma_common.c"
    "${ProjDirPath}/../../../../platform/hal/src/enet/fsl_enet_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_enet_hal.h"
    "${ProjDirPath}/../../../../platform/drivers/src/enet/fsl_enet_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_enet_driver.h"
    "${ProjDirPath}/../../../../platform/drivers/src/enet/fsl_enet_common.c"
    "${ProjDirPath}/../../../../platform/hal/src/dspi/fsl_dspi_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_dspi_hal.h"
    "${ProjDirPath}/../../../../platform/drivers/src/dspi/fsl_dspi_shared_function.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_dspi_shared_function.h"
    "${ProjDirPath}/../../../../platform/drivers/src/dspi/fsl_dspi_common.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_dspi_master_driver.h"
    "${ProjDirPath}/../../../../platform/drivers/src/dspi/fsl_dspi_master_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_dspi_slave_driver.h"
    "${ProjDirPath}/../../../../platform/drivers/src/dspi/fsl_dspi_slave_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_dspi_edma_master_driver.h"
    "${ProjDirPath}/../../../../platform/drivers/src/dspi/fsl_dspi_edma_master_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_dspi_edma_slave_driver.h"
    "${ProjDirPath}/../../../../platform/drivers/src/dspi/fsl_dspi_edma_slave_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/src/dspi/fsl_dspi_edma_shared_function.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_dspi_edma_shared_function.h"
    "${ProjDirPath}/../../../../platform/hal/src/ftm/fsl_ftm_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_ftm_hal.h"
    "${ProjDirPath}/../../../../platform/drivers/src/ftm/fsl_ftm_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_ftm_driver.h"
    "${ProjDirPath}/../../../../platform/drivers/src/ftm/fsl_ftm_common.c"
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
    "${ProjDirPath}/../../../../platform/hal/src/mpu/fsl_mpu_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_mpu_hal.h"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_mpu_driver.h"
    "${ProjDirPath}/../../../../platform/drivers/src/mpu/fsl_mpu_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/src/mpu/fsl_mpu_common.c"
    "${ProjDirPath}/../../../../platform/hal/src/ewm/fsl_ewm_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_ewm_hal.h"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_ewm_driver.h"
    "${ProjDirPath}/../../../../platform/drivers/src/ewm/fsl_ewm_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/src/ewm/fsl_ewm_common.c"
    "${ProjDirPath}/../../../../platform/hal/src/mcg/fsl_mcg_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_mcg_hal.h"
    "${ProjDirPath}/../../../../platform/hal/src/mcg/fsl_mcg_hal_modes.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_mcg_hal_modes.h"
    "${ProjDirPath}/../../../../platform/hal/src/osc/fsl_osc_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_osc_hal.h"
    "${ProjDirPath}/../../../../platform/hal/src/pdb/fsl_pdb_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_pdb_hal.h"
    "${ProjDirPath}/../../../../platform/drivers/src/pdb/fsl_pdb_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_pdb_driver.h"
    "${ProjDirPath}/../../../../platform/drivers/src/pdb/fsl_pdb_common.c"
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
    "${ProjDirPath}/../../../../platform/hal/src/rnga/fsl_rnga_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_rnga_hal.h"
    "${ProjDirPath}/../../../../platform/drivers/src/rnga/fsl_rnga_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_rnga_driver.h"
    "${ProjDirPath}/../../../../platform/drivers/src/rnga/fsl_rnga_common.c"
    "${ProjDirPath}/../../../../platform/hal/src/sai/fsl_sai_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_sai_hal.h"
    "${ProjDirPath}/../../../../platform/drivers/src/sai/fsl_sai_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_sai_driver.h"
    "${ProjDirPath}/../../../../platform/drivers/src/sai/fsl_sai_common.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_sdhc_hal.h"
    "${ProjDirPath}/../../../../platform/hal/src/sdhc/fsl_sdhc_hal.c"
    "${ProjDirPath}/../../../../platform/drivers/src/sdhc/fsl_sdhc_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_sdhc_driver.h"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_sdhc.h"
    "${ProjDirPath}/../../../../platform/composite/inc/fsl_sdmmc_card.h"
    "${ProjDirPath}/../../../../platform/drivers/src/sdhc/fsl_sdhc_common.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_sim_hal.h"
    "${ProjDirPath}/../../../../platform/hal/src/sim/MK60D10/fsl_sim_hal_MK60D10.c"
    "${ProjDirPath}/../../../../platform/hal/src/sim/MK60D10/fsl_sim_hal_MK60D10.h"
    "${ProjDirPath}/../../../../platform/hal/src/smc/fsl_smc_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_smc_hal.h"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_pmc_hal.h"
    "${ProjDirPath}/../../../../platform/hal/src/rcm/fsl_rcm_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_rcm_hal.h"
    "${ProjDirPath}/../../../../platform/hal/src/tsi/fsl_tsi_hal.c"
    "${ProjDirPath}/../../../../platform/hal/src/tsi/fsl_tsi_v2_hal_specific.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_tsi_hal.h"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_tsi_v2_hal_specific.h"
    "${ProjDirPath}/../../../../platform/drivers/src/tsi/fsl_tsi_common.c"
    "${ProjDirPath}/../../../../platform/drivers/src/tsi/fsl_tsi_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/src/tsi/fsl_tsi_v2_driver_specific.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_tsi_driver.h"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_uart_hal.h"
    "${ProjDirPath}/../../../../platform/hal/src/uart/fsl_uart_hal.c"
    "${ProjDirPath}/../../../../platform/drivers/src/uart/fsl_uart_common.c"
    "${ProjDirPath}/../../../../platform/drivers/src/uart/fsl_uart_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_uart_driver.h"
    "${ProjDirPath}/../../../../platform/hal/src/wdog/fsl_wdog_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_wdog_hal.h"
    "${ProjDirPath}/../../../../platform/drivers/src/wdog/fsl_wdog_driver.c"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_wdog_driver.h"
    "${ProjDirPath}/../../../../platform/drivers/src/wdog/fsl_wdog_common.c"
    "${ProjDirPath}/../../../../platform/hal/src/vref/fsl_vref_hal.c"
    "${ProjDirPath}/../../../../platform/hal/inc/fsl_vref_hal.h"
    "${ProjDirPath}/../../../../platform/drivers/inc/fsl_vref_driver.h"
    "${ProjDirPath}/../../../../platform/drivers/src/vref/fsl_vref_common.c"
    "${ProjDirPath}/../../../../platform/drivers/src/vref/fsl_vref_driver.c"
)

SET_TARGET_PROPERTIES(KsdkFreertosLib PROPERTIES OUTPUT_NAME "libksdk_platform_freertos.a")
