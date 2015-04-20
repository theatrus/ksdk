/**HEADER********************************************************************
* 
* Copyright (c) 2010, 2013 - 2014 Freescale Semiconductor;
* All Rights Reserved
*
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
* $FileName: io_serl_int.h$
* $Version : 
* $Date    : 
*
* Comments:
*
*   
*
*END************************************************************************/

#define _CODE_PTR_ *
/* File ops definitions */
#define fopen(fd_ptr, open_name_ptr, flags) _io_serial_int_open(fd_ptr, open_name_ptr, flags)
#define fread(data_ptr,so,no,fd_ptr)        (_io_serial_int_read(fd_ptr, data_ptr, (so)*(no))/(so))
#define fwrite(data_ptr,so,no,fd_ptr)       (_io_serial_int_write(fd_ptr, data_ptr, (so)*(no))/(so))
#define ioctl(fd_ptr, cmd, param_ptr)       _io_serial_int_ioctl(fd_ptr, cmd, param_ptr)
#define fclose(fd_ptr)                      _io_serial_int_close(fd_ptr)

#ifdef stdin
#undef stdin
#endif
#define stdin  (serl_int_get_fdev())

#ifdef stdout
#undef stdout
#endif
#define stdout (serl_int_get_fdev())

/*
 * FILE DEVICE STRUCTURE
 *
 * This structure is used by the current I/O Subsystem to store
 * state information.
 * Use the same structure as the formatted I/O.
 */
 typedef struct
{
    
    /*! \brief The address of the Device for this stream. */
    void * dev_ptr;

    /*! \brief Device Driver specific information. */
    void * dev_data_ptr;

    /*! \brief General control flags for this stream. */
    uint32_t     flags;
    
    /*! \brief The current error for this stream. */
    uint32_t     error;

    /*! \brief The current position in the stream. */
    uint32_t    location;

    /*! \brief The current size of the file. */
    uint32_t    size;

    /*! \brief Undelete implementation. */
    bool        have_ungot_character;
    /*! \brief Undelete implementation. */
    uint32_t     ungot_character;

}file_device_struct_t;

/* Function Declaration */
file_device_struct_t * serl_int_get_fdev(void);
extern uint32_t _io_serial_int_install(
    char     *identifier,
    uint32_t (_CODE_PTR_ init)(void *, char *),
    uint32_t (_CODE_PTR_ enable_ints)(void *),
    uint32_t (_CODE_PTR_ deinit)(void *, void *),
    void     (_CODE_PTR_  putc)(void *, char),
    uint32_t (_CODE_PTR_ ioctl)(void *, uint32_t, void *),
    void     *init_data_ptr,
    uint32_t queue_size
);
extern int32_t _io_serial_int_ioctl(
    file_device_struct_t * fd_ptr,
    uint32_t cmd,
    void *param_ptr
);
extern int32_t _io_serial_int_write(
    file_device_struct_t * fd_ptr,
    char              *data_ptr,
    int32_t               num
);
extern int32_t _io_serial_int_read(
    file_device_struct_t * fd_ptr,
    char              *data_ptr,
    int32_t               num
);
void sci_init(void);

/* EOF */
