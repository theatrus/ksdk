/**HEADER********************************************************************
*
* Copyright (c) 2008, 2014 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2008 Embedded Access Inc.;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
* All Rights Reserved
*
***************************************************************************
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: serl_int_kuart.c$
* $Version : 3.8.10.0$
* $Date    : Jul-3-2012$
*
* Comments:
*
*   This file contains the low level functions for the interrupt driven
*   serial I/O for the SCI device.
*
*END************************************************************************/
//#include "types.h"
//#include "user_config.h"
//#include "bsp.h"
	
//#include "derivative.h"
//#include "MK64F12.h"
#include "adapter.h"

#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "serial.h"
#include "io_serl_int.h"
#include "io_prv.h"
#include "charq.h"
#include "serinprv.h"
#include "serl_kuart.h"

#include "fsl_uart_driver.h"
#include "fsl_interrupt_manager.h"
#include "board.h"


//extern IRQn_Type uart_irq_ids[5];
//const UART_Type * uart_base_addr_cdc[] = UART_BASE_PTRS;

static inline uint32_t __get_uart_baseaddr(uint32_t uartInstance)
{
	return (uint32_t)g_uartBaseAddr[uartInstance];
}

/* Table to save uart IRQ enum numbers defined in CMSIS header file */
extern const IRQn_Type g_uartRxTxIrqId[HW_UART_INSTANCE_COUNT];

/* Polled functions used */
extern uint32_t _kuart_polled_init(kuart_init_struct_t *, void **, char *);
extern uint32_t _kuart_polled_deinit(kuart_init_struct_t *, kuart_info_struct_t *);
extern uint32_t _kuart_polled_ioctl(kuart_info_struct_t *, uint32_t, uint32_t *);
/* Interrupt driver functions */

extern void    _kuart_int_putc(io_serial_int_device_struct_t *, char);
extern uint32_t _kuart_int_init(io_serial_int_device_struct_t *, char *);
extern uint32_t _kuart_int_deinit(kuart_init_struct_t *, kuart_info_struct_t *);
extern uint32_t _kuart_int_enable(kuart_info_struct_t *);
extern void    _kuart_int_err_isr(void *);
extern void    _kuart_int_rx_tx_isr(void *);
void UART_RX_TX_IRQHandler(void);

static kuart_info_struct_t kuart_info = {{0}};

typedef void (*int_isr_fptr_t)(void*);
static int_isr_fptr_t lowlevel_uart_isr = NULL;
static void* isr_param_uart_isr = NULL;

uart_state_t uartState;
static uint32_t uartInstance;


static kuart_init_struct_t _bsp_sci_init = {
	/* queue size		  */ 64,
	/* Channel			  */ BOARD_DEBUG_UART_INSTANCE,
	/* Clock Speed		  */ 120000 * 1000,	 /* SCI0 operates only on system clock */
	/* Baud rate		  */ 115200,
	/* RX/TX Int vect	  */ 0xFFFFFFFF,
	/* ERR Int vect 	  */ 0xFFFFFFFF,
	/* RX/TX priority	  */ 3,
	/* ERR priority 	  */ 4
};

io_serial_int_device_struct_t sci_int_dev = 
{
	   /* The I/O init function */
	   (uint32_t (*)(void*, char*))_kuart_int_init,
	
	   /* The enable interrupts function */
	   (uint32_t (*)(void*))_kuart_int_enable,
	
	   /* The I/O deinit function */
	  (uint32_t (*)(void*, void*))_kuart_int_deinit,
	
	   /* The output function, used to write out the first character */
	   (void (*)(void*, char))_kuart_int_putc,
	
	   /* The ioctl function, (change bauds etc) */
	   NULL,
	
	   /* The I/O channel initialization data */
	   (void*)&_bsp_sci_init,
	   
	   /* Device specific information */
	   NULL,
	
	   /* The queue size to use */
	   64,
	   
	   /* Open count for number of accessing file descriptors */
	   0,
	
	   /* Open flags for this channel */
	   0,
	
	   /* The input queue */
	   NULL,
	
	   /* The input waiting tasks */
	   NULL,
	
	   /* The output queue */
	   NULL,
	
	   /* The output waiting tasks */
	   NULL,
	
	   /* Has output been started */
	   0,
	   
	   /* Protocol flag information */
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
	   0,
};




/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _kuart_int_install
* Returned Value   : uint32_t a task error code or IO_OK
* Comments         :
*    Install an interrupt driven uart serial device.
*
*END*----------------------------------------------------------------------*/

uint32_t _kuart_int_install
   (
      /* [IN] A string that identifies the device for fopen */
      char *identifier,

      /* [IN] The I/O init data pointer */
      kuart_init_struct_cptr  init_data_ptr,

      /* [IN] The I/O queue size to use */
      uint32_t  queue_size
   )
{ /* Body */

#if PE_LDD_VERSION
    if (PE_PeripheralUsed((uint32_t)_bsp_get_serial_base_address(init_data_ptr->device)))
    {
        return IO_ERROR;
    }
#endif

   return _io_serial_int_install(identifier,
      (uint32_t (_CODE_PTR_)(void *, char *))_kuart_int_init,
      (uint32_t (_CODE_PTR_)(void *))_kuart_int_enable,
      (uint32_t (_CODE_PTR_)(void *,void *))_kuart_int_deinit,
      (void    (_CODE_PTR_)(void *, char))_kuart_int_putc,
      (uint32_t (_CODE_PTR_)(void *, uint32_t, void *))_kuart_polled_ioctl,
      (void *)init_data_ptr, queue_size);

} /* Endbody */

/*FUNCTION****************************************************************
*
* Function Name    : _kuart_polled_init
* Returned Value   : SERL_INT_OK or a MQX error code.
* Comments         :
*    This function initializes the SCI
*
*END*********************************************************************/

uint32_t _kuart_polled_init
   (
      /* [IN] the initialization information for the device being opened */
      kuart_init_struct_t *               io_init_ptr,

      /* [OUT] the address to store device specific information */
      void                          **io_info_ptr_ptr,

      /* [IN] the rest of the name of the device opened */
      char                           *open_name_ptr
   )
{ /* Body */
   kuart_info_struct_t *                sci_info_ptr;
   uart_user_config_t                    uartConfig;
   
   uartConfig.bitCountPerChar = kUart8BitsPerChar;
   uartConfig.parityMode = kUartParityDisabled;
   uartConfig.stopBitCount = kUartOneStopBit;
   uartConfig.baudRate = 115200;

   /* Get peripheral address */
   uartInstance = io_init_ptr->device;

   sci_info_ptr = &kuart_info;
   sci_info_ptr->init.device = io_init_ptr->device;
   *io_info_ptr_ptr = sci_info_ptr;

   UART_DRV_Init(uartInstance, &uartState, &uartConfig);

   return SERL_INT_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : _kuart_polled_deinit
* Returned Value   : MQX_OK or a mqx error code.
* Comments         :
*    This function de-initializes the SCI.
*
*END*********************************************************************/

uint32_t _kuart_polled_deinit
   (
      /* [IN] the initialization information for the device being opened */
      kuart_init_struct_t * io_init_ptr,

      /* [IN] the address of the device specific information */
      kuart_info_struct_t * io_info_ptr
   )
{
//TO DO
	return 0;
}

/*FUNCTION****************************************************************
*
* Function Name    : _kuart_polled_ioctl
* Returned Value   : uint32_t MQX_OK or a mqx error code.
* Comments         :
*    This function performs miscellaneous services for
*    the I/O device.
*
*END*********************************************************************/

uint32_t _kuart_polled_ioctl
   (
      /* [IN] the address of the device specific information */
      kuart_info_struct_t * io_info_ptr,

      /* [IN] The command to perform */
      uint32_t                    cmd,

      /* [IN] Parameters for the command */
      uint32_t                *param_ptr
   )
{ /* Body */
	return IO_OK;
}


int32_t soc_install_isr( uint32_t vector, int_isr_fptr_t isr_ptr, void* isr_data)
{
    

     lowlevel_uart_isr = isr_ptr;
     isr_param_uart_isr = isr_data;
   
    return 1;
}

/*FUNCTION****************************************************************
*
* Function Name    : _kuart_int_init
* Returned Value   : uint32_t a task error code or IO_OK
* Comments         :
*    This function initializes the SCI in interrupt mode.
*
*END*********************************************************************/

uint32_t _kuart_int_init
   (
      /* [IN] the interrupt I/O initialization information */
      io_serial_int_device_struct_t * int_io_dev_ptr,

      /* [IN] the rest of the name of the device opened */
      char                       *open_name_ptr
   )
{ /* Body */
   kuart_init_struct_t *     sci_init_ptr;
   uint32_t                  result;

   sci_init_ptr = int_io_dev_ptr->dev_init_data_ptr;
   result = _kuart_polled_init((void *)sci_init_ptr, &int_io_dev_ptr->dev_info_ptr, open_name_ptr);
 
   
   if (result != IO_OK) {
       return(result);
   }/* Endif */
   
   _bsp_sci_init.rx_tx_vector = g_uartRxTxIrqId[_bsp_sci_init.device];
   //soc_install_isr(uart_irq_ids[sci_init_ptr->device], _kuart_int_rx_tx_isr, int_io_dev_ptr);
   //INT_SYS_EnableIRQ(uart_irq_ids[sci_init_ptr->device]);
   soc_install_isr(_bsp_sci_init.rx_tx_vector, _kuart_int_rx_tx_isr, int_io_dev_ptr);
   INT_SYS_EnableIRQ((IRQn_Type)_bsp_sci_init.rx_tx_vector);
   OSA_InstallIntHandler(_bsp_sci_init.rx_tx_vector, UART_RX_TX_IRQHandler);
#if defined (FSL_RTOS_FREE_RTOS)
   NVIC_SetPriority((IRQn_Type)_bsp_sci_init.rx_tx_vector,3);
#endif
   return(IO_OK);

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _kuart_int_deinit
* Returned Value   : uint32_t a task error code or IO_OK
* Comments         :
*    This function de-initializes the UART in interrupt mode.
*
*END*********************************************************************/

uint32_t _kuart_int_deinit
   (
      /* [IN] the interrupt I/O initialization information */
      kuart_init_struct_t * io_init_ptr,

      /* [IN] the address of the device specific information */
      kuart_info_struct_t * io_info_ptr
   )
{ /* Body */
   return(IO_OK);
} /* Endbody */

/*FUNCTION****************************************************************
*
* Function Name    : _kuart_int_enable
* Returned Value   : uint32_t a task error code or IO_OK
* Comments         :
*    This function enables the UART interrupts mode.
*
*END*********************************************************************/

uint32_t _kuart_int_enable
   (
      /* [IN] the address of the device specific information */
      kuart_info_struct_t * io_info_ptr
   )
{ /* Body */

     uartInstance = io_info_ptr->init.device;
     UART_HAL_EnableTransmitter(__get_uart_baseaddr(uartInstance));
     UART_HAL_EnableReceiver(__get_uart_baseaddr(uartInstance));
     UART_HAL_SetRxDataRegFullIntCmd(__get_uart_baseaddr(uartInstance), 1);


   return IO_OK;

} /* Endbody */



/*FUNCTION****************************************************************
*
* Function Name    : _kuart_int_rx_tx_isr
* Returned Value   : none
* Comments         :
*   interrupt handler for the serial input interrupts.
*
*************************************************************************/

void _kuart_int_rx_tx_isr
   (
      /* [IN] the address of the device specific information */
      void   *parameter
   )
{ /* Body */
   io_serial_int_device_struct_t *        int_io_dev_ptr = parameter;
   kuart_info_struct_t *                  sci_info_ptr = int_io_dev_ptr->dev_info_ptr;
   volatile int32_t                        c;

   /*
   if "framing error" or "overrun" error occurs
   perform 'S1' cleanup. if not, 'S1' cleanup will be 
   performed during regular reading of 'D' register.
   */
#if 0
   //if (stat & (UART_S1_OR_MASK | UART_S1_FE_MASK))
   if(UART_HAL_IsReceiveOverrunDetected(uartInstance) || UART_HAL_IsFrameErrorDetected(uartInstance))
   {
      
      // reading register 'D' to cleanup 'S1' may cause 'RFIFO' underflow
      //sci_ptr->D;
      HW_UART_D_RD(uart_base_addr_cdc[uartInstance]);

   
      if(UART_HAL_IsRxFifoUnderflow(uartInstance))
      {
         UART_HAL_FlushRxFifo(uartInstance);
         UART_HAL_ClearTxFifoOverflow(uartInstance);
      }

      // set errno. transmit is corrupted, nothing to process
      // TODO: add valid errno. IO_ERROR causes signed/unsigned assignment warning
      // _task_set_error(IO_ERROR);
      return;
   }
#endif
 
   if(UART_HAL_IsRxDataRegFull(__get_uart_baseaddr(uartInstance)))
   {
      c = HW_UART_D_RD(__get_uart_baseaddr(uartInstance));
      if (!_io_serial_int_addc(int_io_dev_ptr, c)) {
          sci_info_ptr->rx_dropped_input++;
      }
      sci_info_ptr->rx_chars++;

   }



   if(UART_HAL_IsTxDataRegEmpty(__get_uart_baseaddr(uartInstance)))
   {
      c = _io_serial_int_nextc(int_io_dev_ptr);
      if (c >= 0)  {
		uartState.isTxBusy = TRUE;
        UART_HAL_Putchar(__get_uart_baseaddr(uartInstance),c);
      }
      else {
        //UART_HAL_DisableTxDataRegisterEmptyInterrupt(uartInstance);
        UART_HAL_SetTxDataRegEmptyIntCmd(__get_uart_baseaddr(uartInstance),0);
		uartState.isTxBusy = FALSE;
      }
   }

   

}  /* Endbody */

/*FUNCTION****************************************************************
*
* Function Name    : _kuart_int_putc
* Returned Value   : none
* Comments         :
*   This function is called to write out the first character, when
* the output serial device and output ring buffers are empty.
*
*END*********************************************************************/

void _kuart_int_putc
   (
      /* [IN] the address of the device specific information */
      io_serial_int_device_struct_t * int_io_dev_ptr,

      /* [IN] the character to write out now */
      char                       c
   )
{ /* Body */

   UART_HAL_Putchar(__get_uart_baseaddr(uartInstance), c);
   UART_HAL_SetTxDataRegEmptyIntCmd(__get_uart_baseaddr(uartInstance), 1);
   

} /* Endbody */



void UART_RX_TX_IRQHandler(void)
{
      if(lowlevel_uart_isr != NULL)
      lowlevel_uart_isr(isr_param_uart_isr);
}

/* EOF */
