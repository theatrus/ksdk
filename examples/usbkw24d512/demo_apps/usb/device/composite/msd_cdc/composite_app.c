/**HEADER********************************************************************
 * 
 * Copyright (c) 2004 - 2010, 2013 - 2014 Freescale Semiconductor;
 * All Rights Reserved
 *
 *
 *************************************************************************** 
 *
 * @file composite_app.c
 *
 * @author
 *
 * @version
 *
 * @date 
 *
 * @brief  
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device_stack_interface.h"
#include "virtual_com.h"    /* CDC Application Header File */
#include "disk.h"           /* MSD Application Header File */
#include "usb_class_composite.h"
#include "composite_app.h"

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
#include "board.h"
#include "fsl_debug_console.h"

#include "fsl_port_hal.h"
#include <stdio.h>
#include <stdlib.h>
#endif

#if ! USBCFG_DEV_COMPOSITE
#error This application requires USBCFG_DEV_COMPOSITE defined non-zero in usb_device_config.h. Please recompile usbd with this option.
#endif

/*****************************************************************************
 * Constant and Macro's - None
 *****************************************************************************/

/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/

/****************************************************************************
 * Global Variables
 ****************************************************************************/
extern usb_desc_request_notify_struct_t desc_callback;
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
composite_device_struct_t* g_composite_device = NULL;
#else
composite_device_struct_t g_composite_device;
#endif
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
extern void Main_Task(uint32_t param);
#define MAIN_TASK       10

TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
    { MAIN_TASK, Main_Task, 2 * 3000L, 7L, "Main", MQX_AUTO_START_TASK, 0, 0 },
    { 0L, 0L, 0L, 0L, 0L, 0L, 0, 0 }
};
#endif
/*****************************************************************************
 * Local Types - None
 *****************************************************************************/

/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Local Variables 
 *****************************************************************************/

/*****************************************************************************
 * Local Functions
 *****************************************************************************/
/*****************************************************************************
 *  
 *    @name         APP_init
 * 
 *    @brief         This function do initialization for APP.
 * 
 *    @param         None
 * 
 *    @return      None
 **                  
 *****************************************************************************/
void APP_init()
{
    class_config_struct_t* cdc_vcom_config_callback_handle;
    class_config_struct_t* msc_disk_config_callback_handle;

    cdc_vcom_preinit();
    msc_disk_preinit();
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
    if (NULL == g_composite_device)
    {
        g_composite_device = OS_Mem_alloc_uncached_zero(sizeof(composite_device_struct_t));
    }

    /* cdc vcom device */
    cdc_vcom_config_callback_handle = &g_composite_device->composite_device_config_list[CDC_VCOM_INTERFACE_INDEX];
    cdc_vcom_config_callback_handle->composite_application_callback.callback = VCom_USB_App_Device_Callback;
    cdc_vcom_config_callback_handle->composite_application_callback.arg = &g_composite_device->cdc_vcom;
    cdc_vcom_config_callback_handle->class_specific_callback.callback = (usb_class_specific_handler_func) VCom_USB_App_Class_Callback;
    cdc_vcom_config_callback_handle->class_specific_callback.arg = &g_composite_device->cdc_vcom;
    cdc_vcom_config_callback_handle->desc_callback_ptr = &desc_callback;
    cdc_vcom_config_callback_handle->type = USB_CLASS_CDC;

    /* msc disk device */
    msc_disk_config_callback_handle = &g_composite_device->composite_device_config_list[MSC_DISK_INTERFACE_INDEX];
    msc_disk_config_callback_handle->composite_application_callback.callback = Disk_USB_App_Device_Callback;
    msc_disk_config_callback_handle->composite_application_callback.arg = &g_composite_device->msc_disk;
    msc_disk_config_callback_handle->class_specific_callback.callback = (usb_class_specific_handler_func) Disk_USB_App_Class_Callback;
    msc_disk_config_callback_handle->class_specific_callback.arg = &g_composite_device->msc_disk;
    msc_disk_config_callback_handle->desc_callback_ptr = &desc_callback;
    msc_disk_config_callback_handle->type = USB_CLASS_MSC;
    OS_Mem_zero(&g_composite_device->msc_disk, sizeof(disk_struct_t));

    g_composite_device->composite_device_config_callback.count = 2;
    g_composite_device->composite_device_config_callback.class_app_callback = g_composite_device->composite_device_config_list;

    msc_disk_init(&g_composite_device->msc_disk);

    /* Initialize the USB interface */
    USB_Composite_Init(CONTROLLER_ID, &g_composite_device->composite_device_config_callback, &g_composite_device->composite_device);

    g_composite_device->cdc_vcom = (cdc_handle_t) g_composite_device->composite_device_config_list[CDC_VCOM_INTERFACE_INDEX].class_handle;
    g_composite_device->msc_disk.app_handle = (msd_handle_t) g_composite_device->composite_device_config_list[MSC_DISK_INTERFACE_INDEX].class_handle;
    cdc_vcom_init(&g_composite_device->cdc_vcom);
#else   
    /* cdc vcom device */
    cdc_vcom_config_callback_handle = &g_composite_device.composite_device_config_list[CDC_VCOM_INTERFACE_INDEX];
    cdc_vcom_config_callback_handle->composite_application_callback.callback = VCom_USB_App_Device_Callback;
    cdc_vcom_config_callback_handle->composite_application_callback.arg = &g_composite_device.cdc_vcom;
    cdc_vcom_config_callback_handle->class_specific_callback.callback = (usb_class_specific_handler_func)VCom_USB_App_Class_Callback;
    cdc_vcom_config_callback_handle->class_specific_callback.arg = &g_composite_device.cdc_vcom;
    cdc_vcom_config_callback_handle->desc_callback_ptr = &desc_callback;
    cdc_vcom_config_callback_handle->type = USB_CLASS_CDC;

    /* msc disk device */
    msc_disk_config_callback_handle = &g_composite_device.composite_device_config_list[MSC_DISK_INTERFACE_INDEX];
    msc_disk_config_callback_handle->composite_application_callback.callback = Disk_USB_App_Device_Callback;
    msc_disk_config_callback_handle->composite_application_callback.arg = &g_composite_device.msc_disk;
    msc_disk_config_callback_handle->class_specific_callback.callback = (usb_class_specific_handler_func)Disk_USB_App_Class_Callback;
    msc_disk_config_callback_handle->class_specific_callback.arg = &g_composite_device.msc_disk;
    msc_disk_config_callback_handle->desc_callback_ptr = &desc_callback;
    msc_disk_config_callback_handle->type = USB_CLASS_MSC;
    OS_Mem_zero(&g_composite_device.msc_disk, sizeof(disk_struct_t));

    g_composite_device.composite_device_config_callback.count = 2;
    g_composite_device.composite_device_config_callback.class_app_callback = g_composite_device.composite_device_config_list;

    msc_disk_init(&g_composite_device.msc_disk);

    /* Initialize the USB interface */
    USB_Composite_Init(CONTROLLER_ID, &g_composite_device.composite_device_config_callback, &g_composite_device.composite_device);

    g_composite_device.cdc_vcom = (cdc_handle_t)g_composite_device.composite_device_config_list[CDC_VCOM_INTERFACE_INDEX].class_handle;
    g_composite_device.msc_disk.app_handle = (msd_handle_t)g_composite_device.composite_device_config_list[MSC_DISK_INTERFACE_INDEX].class_handle;
    cdc_vcom_init(&g_composite_device.cdc_vcom);
#endif
}
/*****************************************************************************
 *  
 *    @name         APP_task
 * 
 *    @brief         This function runs APP task.
 *    @param         None
 * 
 *    @return      None
 **                  
 *****************************************************************************/
void APP_task()
{
    while (TRUE)
    {
        msc_disk_task();
        cdc_vcom_task();
    }
}

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)

/*FUNCTION*----------------------------------------------------------------
 * 
 * Function Name  : Main_Task
 * Returned Value : None
 * Comments       :
 *     First function called.  Calls the Test_App
 *     callback functions.
 * 
 *END*--------------------------------------------------------------------*/
void Main_Task(uint32_t param)
{
    UNUSED_ARGUMENT (param)
    APP_init();
    APP_task();
}
#endif

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)

#if defined(FSL_RTOS_MQX)
void Main_Task(uint32_t param);
TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
    {   1L, Main_Task, 2500L, MQX_MAIN_TASK_PRIORITY, "Main", MQX_AUTO_START_TASK},
    {   0L, 0L, 0L, 0L, 0L, 0L}
};
#endif

static void Task_Start(void *arg)
{
    APP_init();

    APP_task();
    OSA_TaskDestroy (OSA_TaskGetHandler());
    }
#if defined(FSL_RTOS_MQX)
    void Main_Task(uint32_t param)
#else

#if defined(__CC_ARM) || defined(__GNUC__)
    int main(void)
#else
void main(void)
#endif

#endif
{
    hardware_init();
    OSA_Init();
    dbg_uart_init();

    OS_Task_create(Task_Start, NULL, 9L, 3000L, "task_start", NULL);

    OSA_Start();
#if (!defined(FSL_RTOS_MQX))&(defined(__CC_ARM) || defined(__GNUC__))
    return 1;
#endif
}
#endif
/* EOF */
