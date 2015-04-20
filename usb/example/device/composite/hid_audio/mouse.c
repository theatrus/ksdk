/**HEADER********************************************************************
* 
* Copyright (c) 2008, 2013 Freescale Semiconductor;
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
* $FileName: mouse.c$
* $Version : 
* $Date    : 
*
* Comments:
*
* @brief  The file emulates a mouse cursor movements
*         
*****************************************************************************/
 
/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device_stack_interface.h"
#include "mouse.h"



/*****************************************************************************
 * Constant and Macro's - None
 *****************************************************************************/
 
/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/


/****************************************************************************
 * Global Variables
 ****************************************************************************/
/* Add all the variables needed for mouse.c to this structure */
hid_mouse_struct_t*  g_hid_mouse_ptr;

/*****************************************************************************
 * Local Types - None
 *****************************************************************************/
 
/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
void Hid_USB_App_Device_Callback(uint8_t event_type, void* val,void* arg); 
uint8_t Hid_USB_App_Class_Callback(uint8_t request, uint16_t value, uint8_t ** data, 
    uint32_t* size,void* arg); 
/*****************************************************************************
 * Local Variables 
 *****************************************************************************/

/*****************************************************************************
 * Local Functions
 *****************************************************************************/

/*****************************************************************************
* 
*      @name     move_mouse
*
*      @brief    This function gets makes the cursor on screen move left,right
*                up and down
*
*      @param    None      
*
*      @return   None
*     
* 
******************************************************************************/
void move_mouse(void) 
{
    static int32_t x = 0, y = 0;  
    static enum { RIGHT, DOWN, LEFT, UP } dir = RIGHT;
    
    switch (dir) 
    {
        case RIGHT:
            g_hid_mouse_ptr->rpt_buf[1] = 2; 
            g_hid_mouse_ptr->rpt_buf[2] = 0; 
            x++;
            if (x > 1)
                dir++;
            break;
        case DOWN:
            g_hid_mouse_ptr->rpt_buf[1] = 0; 
            g_hid_mouse_ptr->rpt_buf[2] = 2; 
            y++;
            if (y >1)
                dir++;
            break;
        case LEFT:
            g_hid_mouse_ptr->rpt_buf[1] = (uint8_t)(-2); 
            g_hid_mouse_ptr->rpt_buf[2] = 0; 
            x--;
            if (x < 0)
                dir++;
            
            break;
        case UP:
            g_hid_mouse_ptr->rpt_buf[1] = 0; 
            g_hid_mouse_ptr->rpt_buf[2] = (uint8_t)(-2); 
            y--;
            if (y < 0)
                dir = RIGHT;          
            break;
    }    
    (void)USB_Class_HID_Send_Data(g_hid_mouse_ptr->app_handle,HID_ENDPOINT,
        g_hid_mouse_ptr->rpt_buf,MOUSE_BUFF_SIZE);

                                                              
}
   
/******************************************************************************
 * 
 *    @name        Hid_USB_App_Device_Callback
 *    
 *    @brief       This function handles the callback  
 *                  
 *    @param       handle : handle to Identify the controller
 *    @param       event_type : value of the event
 *    @param       val : gives the configuration value 
 * 
 *    @return      None
 *
 *****************************************************************************/
void Hid_USB_App_Device_Callback(uint8_t event_type, void* val,void* arg) 
{    
    UNUSED_ARGUMENT (arg)
    UNUSED_ARGUMENT (val)
    
    switch(event_type)
    {
        case USB_DEV_EVENT_BUS_RESET:     
            g_hid_mouse_ptr->mouse_init=FALSE;
            break;
        case USB_DEV_EVENT_ENUM_COMPLETE:    
            g_hid_mouse_ptr->mouse_init = TRUE;         
            move_mouse();/* run the coursor movement code */
            break;
        case USB_DEV_EVENT_ERROR: 
            /* user may add code here for error handling 
               NOTE : val has the value of error from h/w*/
            break;
        default: 
            break;          
    }   
    return;
}


/******************************************************************************
 * 
 *    @name        Hid_USB_App_Class_Callback
 *    
 *    @brief       This function handles the callback for Get/Set report req  
 *                  
 *    @param       request  :  request type
 *    @param       value    :  give report type and id
 *    @param       data     :  pointer to the data 
 *    @param       size     :  size of the transfer
 *
 *    @return      status
 *                  USB_OK  :  if successful
 *                  else return error
 *
 *****************************************************************************/
 uint8_t Hid_USB_App_Class_Callback
 (
    uint8_t request, 
    uint16_t value, 
    uint8_t ** data, 
    uint32_t* size,
    void* arg
) 
{
    uint8_t error = USB_OK;
    //uint8_t direction =  (uint8_t)((request & USB_HID_REQUEST_DIR_MASK) >>3);
    uint8_t index = (uint8_t)((request - 2) & USB_HID_REQUEST_TYPE_MASK); 
    if((request == USB_DEV_EVENT_SEND_COMPLETE) && (value == USB_REQ_VAL_INVALID))
    {
      if((g_hid_mouse_ptr->mouse_init)&& (arg != NULL))
      {
          #if COMPLIANCE_TESTING				  
                   uint32_t g_compliance_delay = 0x009FFFFF;
                   while(g_compliance_delay--);
          #endif

          move_mouse();/* run the coursor movement code */						
      }
      return error;
    }
	 
    /* index == 0 for get/set idle, index == 1 for get/set protocol */        
    *size = 0;
    /* handle the class request */
    switch(request) 
    {
        case USB_HID_GET_REPORT_REQUEST :       
            *data = &g_hid_mouse_ptr->rpt_buf[0]; /* point to the report to send */                    
            *size = MOUSE_BUFF_SIZE; /* report size */          
            break;                                        
              
        case USB_HID_SET_REPORT_REQUEST :
            for(index = 0; index < MOUSE_BUFF_SIZE ; index++) 
            {   /* copy the report sent by the host */          
                g_hid_mouse_ptr->rpt_buf[index] = *(*data + index);
            }        
            break;
                 
        case USB_HID_GET_IDLE_REQUEST :
            /* point to the current idle rate */
            *data = &g_hid_mouse_ptr->app_request_params[index];
            *size = REQ_DATA_SIZE;
            break;
                
        case USB_HID_SET_IDLE_REQUEST :
            /* set the idle rate sent by the host */
            g_hid_mouse_ptr->app_request_params[index] =(uint8_t)((value & MSB_MASK) >> 
                HIGH_BYTE_SHIFT);
            break;
           
        case USB_HID_GET_PROTOCOL_REQUEST :
            /* point to the current protocol code 
               0 = Boot Protocol
               1 = Report Protocol*/
            *data = &g_hid_mouse_ptr->app_request_params[index];
            *size = REQ_DATA_SIZE;
            break;
                
        case USB_HID_SET_PROTOCOL_REQUEST :
            /* set the protocol sent by the host 
               0 = Boot Protocol
               1 = Report Protocol*/
               g_hid_mouse_ptr->app_request_params[index] = (uint8_t)(value);  
               break;
    }           
    return error; 
}  
 //#define OS_Mem_zero(ptr,n)                   memset((ptr),(0),(n))
/******************************************************************************
 *  
 *   @name        hid_mouse_init
 * 
 *   @brief       This function is the entry for mouse (or other usuage)
 * 
 *   @param       None
 * 
 *   @return      None
 **                
 *****************************************************************************/
void hid_mouse_init(void* param)
{       
    g_hid_mouse_ptr = (hid_mouse_struct_t*)param;
} 


/* EOF */
