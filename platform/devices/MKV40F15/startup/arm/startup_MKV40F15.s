; * ---------------------------------------------------------------------------------------
; *  @file:    startup_MKV40F15.s
; *  @purpose: CMSIS Cortex-M4 Core Device Startup File
; *            MKV40F15
; *  @version: 1.2
; *  @date:    2015-11-2
; *  @build:   b150211
; * ---------------------------------------------------------------------------------------
; *
; * Copyright (c) 1997 - 2015 , Freescale Semiconductor, Inc.
; * All rights reserved.
; *
; * Redistribution and use in source and binary forms, with or without modification,
; * are permitted provided that the following conditions are met:
; *
; * o Redistributions of source code must retain the above copyright notice, this list
; *   of conditions and the following disclaimer.
; *
; * o Redistributions in binary form must reproduce the above copyright notice, this
; *   list of conditions and the following disclaimer in the documentation and/or
; *   other materials provided with the distribution.
; *
; * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
; *   contributors may be used to endorse or promote products derived from this
; *   software without specific prior written permission.
; *
; * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
; * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
; * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
; * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
; * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
; * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
; * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
; * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
; * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
; * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
; *
; *------- <<< Use Configuration Wizard in Context Menu >>> ------------------
; *
; *****************************************************************************/


                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size
                IMPORT  |Image$$ARM_LIB_STACK$$ZI$$Limit|

__Vectors       DCD     |Image$$ARM_LIB_STACK$$ZI$$Limit| ; Top of Stack
                DCD     Reset_Handler  ; Reset Handler
                DCD     NMI_Handler                         ;NMI Handler
                DCD     HardFault_Handler                   ;Hard Fault Handler
                DCD     MemManage_Handler                   ;MPU Fault Handler
                DCD     BusFault_Handler                    ;Bus Fault Handler
                DCD     UsageFault_Handler                  ;Usage Fault Handler
                DCD     0                                   ;Reserved
                DCD     0                                   ;Reserved
                DCD     0                                   ;Reserved
                DCD     0                                   ;Reserved
                DCD     SVC_Handler                         ;SVCall Handler
                DCD     DebugMon_Handler                    ;Debug Monitor Handler
                DCD     0                                   ;Reserved
                DCD     PendSV_Handler                      ;PendSV Handler
                DCD     SysTick_Handler                     ;SysTick Handler

                                                            ;External Interrupts
                DCD     DMA0_IRQHandler                     ;DMA channel 0, 16 transfer complete
                DCD     DMA1_IRQHandler                     ;DMA channel 1, 17 transfer complete
                DCD     DMA2_IRQHandler                     ;DMA channel 2, 18 transfer complete
                DCD     DMA3_IRQHandler                     ;DMA channel 3, 19 transfer complete
                DCD     DMA4_IRQHandler                     ;DMA channel 4, 20 transfer complete
                DCD     DMA5_IRQHandler                     ;DMA channel 5, 21 transfer complete
                DCD     DMA6_IRQHandler                     ;DMA channel 6, 22 transfer complete
                DCD     DMA7_IRQHandler                     ;DMA channel 7, 23 transfer complete
                DCD     DMA8_IRQHandler                     ;DMA channel 8, 24 transfer complete
                DCD     DMA9_IRQHandler                     ;DMA channel 9, 25 transfer complete
                DCD     DMA10_IRQHandler                    ;DMA channel 10, 26 transfer complete
                DCD     DMA11_IRQHandler                    ;DMA channel 11, 27 transfer complete
                DCD     DMA12_IRQHandler                    ;DMA channel 12, 28 transfer complete
                DCD     DMA13_IRQHandler                    ;DMA channel 13, 29 transfer complete
                DCD     DMA14_IRQHandler                    ;DMA channel 14, 30 transfer complete
                DCD     DMA15_IRQHandler                    ;DMA channel 15, 31 transfer complete
                DCD     DMA_Error_IRQHandler                ;DMA error interrupt channels 0-1531
                DCD     MCM_IRQHandler                      ;MCM interrupt
                DCD     FTFA_IRQHandler                     ;Command complete
                DCD     FTFA_Collision_IRQHandler           ;Read collision
                DCD     PMC_IRQHandler                      ;Low-voltage detect, low-voltage warning
                DCD     LLWU_IRQHandler                     ;Low Leakage Wakeup
                DCD     WDOG_EWM_IRQHandler                 ;Both watchdog modules share this interrupt
                DCD     Reserved39_IRQHandler               ;Reserved interrupt
                DCD     I2C0_IRQHandler                     ;I2C
                DCD     Reserved41_IRQHandler               ;Reserved interrupt
                DCD     SPI_IRQHandler                      ;SPI
                DCD     Reserved43_IRQHandler               ;Reserved interrupt
                DCD     Reserved44_IRQHandler               ;Reserved interrupt
                DCD     Reserved45_IRQHandler               ;Reserved interrupt
                DCD     Reserved46_IRQHandler               ;Reserved interrupt
                DCD     UART0_RX_TX_IRQHandler              ;UART0 status sources
                DCD     UART0_ERR_IRQHandler                ;UART0 error sources
                DCD     UART1_RX_TX_IRQHandler              ;UART1 status sources
                DCD     UART1_ERR_IRQHandler                ;UART1 error sources
                DCD     Reserved51_IRQHandler               ;Reserved interrupt
                DCD     Reserved52_IRQHandler               ;Reserved interrupt
                DCD     Reserved53_IRQHandler               ;Reserved interrupt
                DCD     ADC_ERR_IRQHandler                  ;ADC_ERR A and B ( zero cross, high/low limit)
                DCD     ADCA_IRQHandler                     ;ADCA Scan complete
                DCD     CMP0_IRQHandler                     ;CMP0
                DCD     CMP1_IRQHandler                     ;CMP1
                DCD     FTM0_IRQHandler                     ;FTM0 8 channels
                DCD     FTM1_IRQHandler                     ;FTM1 2 channels
                DCD     Reserved60_IRQHandler               ;Reserved interrupt
                DCD     Reserved61_IRQHandler               ;Reserved interrupt
                DCD     Reserved62_IRQHandler               ;Reserved interrupt
                DCD     Reserved63_IRQHandler               ;Reserved interrupt
                DCD     PIT0_IRQHandler                     ;PIT Channel 0
                DCD     PIT1_IRQHandler                     ;PIT Channel 1
                DCD     PIT2_IRQHandler                     ;PIT Channel 2
                DCD     PIT3_IRQHandler                     ;PIT Channel 3
                DCD     PDB0_IRQHandler                     ;PDB0
                DCD     Reserved69_IRQHandler               ;Reserved interrupt
                DCD     XBARA_IRQHandler                    ;XBARA
                DCD     PDB1_IRQHandler                     ;PDB1
                DCD     Reserved72_IRQHandler               ;Reserved interrupt
                DCD     MCG_IRQHandler                      ;MCG
                DCD     LPTMR0_IRQHandler                   ;LPTMR
                DCD     PORTA_IRQHandler                    ;Pin detect (Port A)
                DCD     PORTB_IRQHandler                    ;Pin detect (Port B)
                DCD     PORTC_IRQHandler                    ;Pin detect (Port C)
                DCD     PORTD_IRQHandler                    ;Pin detect (Port D)
                DCD     PORTE_IRQHandler                    ;Pin detect (Port E)
                DCD     SWI_IRQHandler                      ;Software
                DCD     Reserved81_IRQHandler               ;Reserved interrupt
                DCD     ENC_COMPARE_IRQHandler              ;ENC Compare
                DCD     ENC_HOME_IRQHandler                 ;ENC Home
                DCD     ENC_WDOG_SAB_IRQHandler             ;ENC Watchdog/Simultaneous A and B change
                DCD     ENC_INDEX_IRQHandler                ;ENC Index/Roll over/Roll Under
                DCD     CMP2_IRQHandler                     ;CMP2
                DCD     FTM3_IRQHandler                     ;FTM3 8 channels
                DCD     Reserved88_IRQHandler               ;Reserved interrupt
                DCD     ADCB_IRQHandler                     ;ADCB Scan complete
                DCD     Reserved90_IRQHandler               ;Reserved interrupt
                DCD     CAN0_ORed_Message_buffer_IRQHandler ;FLexCAN0 OR'ed Message buffer (0-15)
                DCD     CAN0_Bus_Off_IRQHandler             ;FLexCAN0 Bus Off
                DCD     CAN0_Error_IRQHandler               ;FLexCAN0 Error
                DCD     CAN0_Tx_Warning_IRQHandler          ;FLexCAN0 Transmit Warning
                DCD     CAN0_Rx_Warning_IRQHandler          ;FLexCAN0 Receive Warning
                DCD     CAN0_Wake_Up_IRQHandler             ;FLexCAN0 Wake Up
                DCD     Reserved97_IRQHandler               ;Reserved interrupt
                DCD     Reserved98_IRQHandler               ;Reserved interrupt
                DCD     Reserved99_IRQHandler               ;Reserved interrupt
                DCD     Reserved100_IRQHandler              ;Reserved interrupt
                DCD     Reserved101_IRQHandler              ;Reserved interrupt
                DCD     Reserved102_IRQHandler              ;Reserved interrupt
                DCD     Reserved103_IRQHandler              ;Reserved interrupt
                DCD     Reserved104_IRQHandler              ;Reserved interrupt
                DCD     Reserved105_IRQHandler              ;Reserved interrupt
                DCD     Reserved106_IRQHandler              ;Reserved interrupt
                DCD     Reserved107_IRQHandler              ;Reserved interrupt
                DCD     CMP3_IRQHandler                     ;CMP3
                DCD     Reserved109_IRQHandler              ;Reserved interrupt
                DCD     CAN1_ORed_Message_buffer_IRQHandler ;FLexCAN1 OR'ed Message buffer (0-15)
                DCD     CAN1_Bus_Off_IRQHandler             ;FLexCAN1 Bus Off
                DCD     CAN1_Error_IRQHandler               ;FLexCAN1 Error
                DCD     CAN1_Tx_Warning_IRQHandler          ;FLexCAN1 Transmit Warning
                DCD     CAN1_Rx_Warning_IRQHandler          ;FLexCAN1 Receive Warning
                DCD     CAN1_Wake_Up_IRQHandler             ;FLexCAN1 Wake Up
                DCD     DefaultISR                          ;116
                DCD     DefaultISR                          ;117
                DCD     DefaultISR                          ;118
                DCD     DefaultISR                          ;119
                DCD     DefaultISR                          ;120
                DCD     DefaultISR                          ;121
                DCD     DefaultISR                          ;122
                DCD     DefaultISR                          ;123
                DCD     DefaultISR                          ;124
                DCD     DefaultISR                          ;125
                DCD     DefaultISR                          ;126
                DCD     DefaultISR                          ;127
                DCD     DefaultISR                          ;128
                DCD     DefaultISR                          ;129
                DCD     DefaultISR                          ;130
                DCD     DefaultISR                          ;131
                DCD     DefaultISR                          ;132
                DCD     DefaultISR                          ;133
                DCD     DefaultISR                          ;134
                DCD     DefaultISR                          ;135
                DCD     DefaultISR                          ;136
                DCD     DefaultISR                          ;137
                DCD     DefaultISR                          ;138
                DCD     DefaultISR                          ;139
                DCD     DefaultISR                          ;140
                DCD     DefaultISR                          ;141
                DCD     DefaultISR                          ;142
                DCD     DefaultISR                          ;143
                DCD     DefaultISR                          ;144
                DCD     DefaultISR                          ;145
                DCD     DefaultISR                          ;146
                DCD     DefaultISR                          ;147
                DCD     DefaultISR                          ;148
                DCD     DefaultISR                          ;149
                DCD     DefaultISR                          ;150
                DCD     DefaultISR                          ;151
                DCD     DefaultISR                          ;152
                DCD     DefaultISR                          ;153
                DCD     DefaultISR                          ;154
                DCD     DefaultISR                          ;155
                DCD     DefaultISR                          ;156
                DCD     DefaultISR                          ;157
                DCD     DefaultISR                          ;158
                DCD     DefaultISR                          ;159
                DCD     DefaultISR                          ;160
                DCD     DefaultISR                          ;161
                DCD     DefaultISR                          ;162
                DCD     DefaultISR                          ;163
                DCD     DefaultISR                          ;164
                DCD     DefaultISR                          ;165
                DCD     DefaultISR                          ;166
                DCD     DefaultISR                          ;167
                DCD     DefaultISR                          ;168
                DCD     DefaultISR                          ;169
                DCD     DefaultISR                          ;170
                DCD     DefaultISR                          ;171
                DCD     DefaultISR                          ;172
                DCD     DefaultISR                          ;173
                DCD     DefaultISR                          ;174
                DCD     DefaultISR                          ;175
                DCD     DefaultISR                          ;176
                DCD     DefaultISR                          ;177
                DCD     DefaultISR                          ;178
                DCD     DefaultISR                          ;179
                DCD     DefaultISR                          ;180
                DCD     DefaultISR                          ;181
                DCD     DefaultISR                          ;182
                DCD     DefaultISR                          ;183
                DCD     DefaultISR                          ;184
                DCD     DefaultISR                          ;185
                DCD     DefaultISR                          ;186
                DCD     DefaultISR                          ;187
                DCD     DefaultISR                          ;188
                DCD     DefaultISR                          ;189
                DCD     DefaultISR                          ;190
                DCD     DefaultISR                          ;191
                DCD     DefaultISR                          ;192
                DCD     DefaultISR                          ;193
                DCD     DefaultISR                          ;194
                DCD     DefaultISR                          ;195
                DCD     DefaultISR                          ;196
                DCD     DefaultISR                          ;197
                DCD     DefaultISR                          ;198
                DCD     DefaultISR                          ;199
                DCD     DefaultISR                          ;200
                DCD     DefaultISR                          ;201
                DCD     DefaultISR                          ;202
                DCD     DefaultISR                          ;203
                DCD     DefaultISR                          ;204
                DCD     DefaultISR                          ;205
                DCD     DefaultISR                          ;206
                DCD     DefaultISR                          ;207
                DCD     DefaultISR                          ;208
                DCD     DefaultISR                          ;209
                DCD     DefaultISR                          ;210
                DCD     DefaultISR                          ;211
                DCD     DefaultISR                          ;212
                DCD     DefaultISR                          ;213
                DCD     DefaultISR                          ;214
                DCD     DefaultISR                          ;215
                DCD     DefaultISR                          ;216
                DCD     DefaultISR                          ;217
                DCD     DefaultISR                          ;218
                DCD     DefaultISR                          ;219
                DCD     DefaultISR                          ;220
                DCD     DefaultISR                          ;221
                DCD     DefaultISR                          ;222
                DCD     DefaultISR                          ;223
                DCD     DefaultISR                          ;224
                DCD     DefaultISR                          ;225
                DCD     DefaultISR                          ;226
                DCD     DefaultISR                          ;227
                DCD     DefaultISR                          ;228
                DCD     DefaultISR                          ;229
                DCD     DefaultISR                          ;230
                DCD     DefaultISR                          ;231
                DCD     DefaultISR                          ;232
                DCD     DefaultISR                          ;233
                DCD     DefaultISR                          ;234
                DCD     DefaultISR                          ;235
                DCD     DefaultISR                          ;236
                DCD     DefaultISR                          ;237
                DCD     DefaultISR                          ;238
                DCD     DefaultISR                          ;239
                DCD     DefaultISR                          ;240
                DCD     DefaultISR                          ;241
                DCD     DefaultISR                          ;242
                DCD     DefaultISR                          ;243
                DCD     DefaultISR                          ;244
                DCD     DefaultISR                          ;245
                DCD     DefaultISR                          ;246
                DCD     DefaultISR                          ;247
                DCD     DefaultISR                          ;248
                DCD     DefaultISR                          ;249
                DCD     DefaultISR                          ;250
                DCD     DefaultISR                          ;251
                DCD     DefaultISR                          ;252
                DCD     DefaultISR                          ;253
                DCD     DefaultISR                          ;254
                DCD     0xFFFFFFFF                          ; Reserved for user TRIM value
__Vectors_End

__Vectors_Size 	EQU     __Vectors_End - __Vectors

; <h> Flash Configuration
;   <i> 16-byte flash configuration field that stores default protection settings (loaded on reset)
;   <i> and security information that allows the MCU to restrict access to the FTFL module.
;   <h> Backdoor Comparison Key
;     <o0>  Backdoor Comparison Key 0.  <0x0-0xFF:2>
;     <o1>  Backdoor Comparison Key 1.  <0x0-0xFF:2>
;     <o2>  Backdoor Comparison Key 2.  <0x0-0xFF:2>
;     <o3>  Backdoor Comparison Key 3.  <0x0-0xFF:2>
;     <o4>  Backdoor Comparison Key 4.  <0x0-0xFF:2>
;     <o5>  Backdoor Comparison Key 5.  <0x0-0xFF:2>
;     <o6>  Backdoor Comparison Key 6.  <0x0-0xFF:2>
;     <o7>  Backdoor Comparison Key 7.  <0x0-0xFF:2>
BackDoorK0      EQU     0xFF
BackDoorK1      EQU     0xFF
BackDoorK2      EQU     0xFF
BackDoorK3      EQU     0xFF
BackDoorK4      EQU     0xFF
BackDoorK5      EQU     0xFF
BackDoorK6      EQU     0xFF
BackDoorK7      EQU     0xFF
;   </h>
;   <h> Program flash protection bytes (FPROT)
;     <i> Each program flash region can be protected from program and erase operation by setting the associated PROT bit.
;     <i> Each bit protects a 1/32 region of the program flash memory.
;     <h> FPROT0
;       <i> Program Flash Region Protect Register 0
;       <i> 1/32 - 8/32 region
;       <o.0>   FPROT0.0
;       <o.1>   FPROT0.1
;       <o.2>   FPROT0.2
;       <o.3>   FPROT0.3
;       <o.4>   FPROT0.4
;       <o.5>   FPROT0.5
;       <o.6>   FPROT0.6
;       <o.7>   FPROT0.7
nFPROT0         EQU     0x00
FPROT0          EQU     nFPROT0:EOR:0xFF
;     </h>
;     <h> FPROT1
;       <i> Program Flash Region Protect Register 1
;       <i> 9/32 - 16/32 region
;       <o.0>   FPROT1.0
;       <o.1>   FPROT1.1
;       <o.2>   FPROT1.2
;       <o.3>   FPROT1.3
;       <o.4>   FPROT1.4
;       <o.5>   FPROT1.5
;       <o.6>   FPROT1.6
;       <o.7>   FPROT1.7
nFPROT1         EQU     0x00
FPROT1          EQU     nFPROT1:EOR:0xFF
;     </h>
;     <h> FPROT2
;       <i> Program Flash Region Protect Register 2
;       <i> 17/32 - 24/32 region
;       <o.0>   FPROT2.0
;       <o.1>   FPROT2.1
;       <o.2>   FPROT2.2
;       <o.3>   FPROT2.3
;       <o.4>   FPROT2.4
;       <o.5>   FPROT2.5
;       <o.6>   FPROT2.6
;       <o.7>   FPROT2.7
nFPROT2         EQU     0x00
FPROT2          EQU     nFPROT2:EOR:0xFF
;     </h>
;     <h> FPROT3
;       <i> Program Flash Region Protect Register 3
;       <i> 25/32 - 32/32 region
;       <o.0>   FPROT3.0
;       <o.1>   FPROT3.1
;       <o.2>   FPROT3.2
;       <o.3>   FPROT3.3
;       <o.4>   FPROT3.4
;       <o.5>   FPROT3.5
;       <o.6>   FPROT3.6
;       <o.7>   FPROT3.7
nFPROT3         EQU     0x00
FPROT3          EQU     nFPROT3:EOR:0xFF
;     </h>
;   </h>
;   <h> Data flash protection byte (FDPROT)
;     <i> Each bit protects a 1/8 region of the data flash memory.
;     <i> (Program flash only devices: Reserved)
;       <o.0>   FDPROT.0
;       <o.1>   FDPROT.1
;       <o.2>   FDPROT.2
;       <o.3>   FDPROT.3
;       <o.4>   FDPROT.4
;       <o.5>   FDPROT.5
;       <o.6>   FDPROT.6
;       <o.7>   FDPROT.7
nFDPROT         EQU     0x00
FDPROT          EQU     nFDPROT:EOR:0xFF
;   </h>
;   <h> EEPROM protection byte (FEPROT)
;     <i> FlexNVM devices: Each bit protects a 1/8 region of the EEPROM.
;     <i> (Program flash only devices: Reserved)
;       <o.0>   FEPROT.0
;       <o.1>   FEPROT.1
;       <o.2>   FEPROT.2
;       <o.3>   FEPROT.3
;       <o.4>   FEPROT.4
;       <o.5>   FEPROT.5
;       <o.6>   FEPROT.6
;       <o.7>   FEPROT.7
nFEPROT         EQU     0x00
FEPROT          EQU     nFEPROT:EOR:0xFF
;   </h>
;   <h> Flash nonvolatile option byte (FOPT)
;     <i> Allows the user to customize the operation of the MCU at boot time.
;     <o.0> LPBOOT
;       <0=> Low-power boot
;       <1=> Normal boot
;     <o.2> NMI_DIS
;       <0=> NMI interrupts are always blocked
;       <1=> NMI_b pin/interrupts reset default to enabled
;     <o.5> FAST_INIT
;       <0=> Slower initialization
;       <1=> Fast Initialization
FOPT          EQU     0xFF
;   </h>
;   <h> Flash security byte (FSEC)
;     <i> WARNING: If SEC field is configured as "MCU security status is secure" and MEEN field is configured as "Mass erase is disabled",
;     <i> MCU's security status cannot be set back to unsecure state since Mass erase via the debugger is blocked !!!
;     <o.0..1> SEC
;       <2=> MCU security status is unsecure
;       <3=> MCU security status is secure
;         <i> Flash Security
;     <o.2..3> FSLACC
;       <2=> Freescale factory access denied
;       <3=> Freescale factory access granted
;         <i> Freescale Failure Analysis Access Code
;     <o.4..5> MEEN
;       <2=> Mass erase is disabled
;       <3=> Mass erase is enabled
;     <o.6..7> KEYEN
;       <2=> Backdoor key access enabled
;       <3=> Backdoor key access disabled
;         <i> Backdoor Key Security Enable
FSEC          EQU     0xFE
;   </h>
; </h>
                IF      :LNOT::DEF:RAM_TARGET
                AREA    FlashConfig, DATA, READONLY
__FlashConfig
                DCB     BackDoorK0, BackDoorK1, BackDoorK2, BackDoorK3
                DCB     BackDoorK4, BackDoorK5, BackDoorK6, BackDoorK7
                DCB     FPROT0    , FPROT1    , FPROT2    , FPROT3
                DCB     FSEC      , FOPT      , FEPROT    , FDPROT
                ENDIF


                AREA    |.text|, CODE, READONLY

; Reset Handler

Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  SystemInit
                IMPORT  init_data_bss
                IMPORT  __main

                IF      :LNOT::DEF:RAM_TARGET
                LDR R0, =FlashConfig    ; dummy read, workaround for flashConfig
                ENDIF

                CPSID   I               ; Mask interrupts
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =init_data_bss
                BLX     R0
                CPSIE   i               ; Unmask interrupts
                LDR     R0, =__main
                BX      R0
                ENDP


; Dummy Exception Handlers (infinite loops which can be modified)
NMI_Handler\
                PROC
                EXPORT  NMI_Handler         [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler         [WEAK]
                B       .
                ENDP
MemManage_Handler\
                PROC
                EXPORT  MemManage_Handler         [WEAK]
                B       .
                ENDP
BusFault_Handler\
                PROC
                EXPORT  BusFault_Handler         [WEAK]
                B       .
                ENDP
UsageFault_Handler\
                PROC
                EXPORT  UsageFault_Handler         [WEAK]
                B       .
                ENDP
SVC_Handler\
                PROC
                EXPORT  SVC_Handler         [WEAK]
                B       .
                ENDP
DebugMon_Handler\
                PROC
                EXPORT  DebugMon_Handler         [WEAK]
                B       .
                ENDP
PendSV_Handler\
                PROC
                EXPORT  PendSV_Handler         [WEAK]
                B       .
                ENDP
SysTick_Handler\
                PROC
                EXPORT  SysTick_Handler         [WEAK]
                B       .
                ENDP
Default_Handler\
                PROC
                EXPORT  DMA0_IRQHandler         [WEAK]
                EXPORT  DMA1_IRQHandler         [WEAK]
                EXPORT  DMA2_IRQHandler         [WEAK]
                EXPORT  DMA3_IRQHandler         [WEAK]
                EXPORT  DMA4_IRQHandler         [WEAK]
                EXPORT  DMA5_IRQHandler         [WEAK]
                EXPORT  DMA6_IRQHandler         [WEAK]
                EXPORT  DMA7_IRQHandler         [WEAK]
                EXPORT  DMA8_IRQHandler         [WEAK]
                EXPORT  DMA9_IRQHandler         [WEAK]
                EXPORT  DMA10_IRQHandler         [WEAK]
                EXPORT  DMA11_IRQHandler         [WEAK]
                EXPORT  DMA12_IRQHandler         [WEAK]
                EXPORT  DMA13_IRQHandler         [WEAK]
                EXPORT  DMA14_IRQHandler         [WEAK]
                EXPORT  DMA15_IRQHandler         [WEAK]
                EXPORT  DMA_Error_IRQHandler         [WEAK]
                EXPORT  MCM_IRQHandler         [WEAK]
                EXPORT  FTFA_IRQHandler         [WEAK]
                EXPORT  FTFA_Collision_IRQHandler         [WEAK]
                EXPORT  PMC_IRQHandler         [WEAK]
                EXPORT  LLWU_IRQHandler         [WEAK]
                EXPORT  WDOG_EWM_IRQHandler         [WEAK]
                EXPORT  Reserved39_IRQHandler         [WEAK]
                EXPORT  I2C0_IRQHandler         [WEAK]
                EXPORT  Reserved41_IRQHandler         [WEAK]
                EXPORT  SPI_IRQHandler         [WEAK]
                EXPORT  Reserved43_IRQHandler         [WEAK]
                EXPORT  Reserved44_IRQHandler         [WEAK]
                EXPORT  Reserved45_IRQHandler         [WEAK]
                EXPORT  Reserved46_IRQHandler         [WEAK]
                EXPORT  UART0_RX_TX_IRQHandler         [WEAK]
                EXPORT  UART0_ERR_IRQHandler         [WEAK]
                EXPORT  UART1_RX_TX_IRQHandler         [WEAK]
                EXPORT  UART1_ERR_IRQHandler         [WEAK]
                EXPORT  Reserved51_IRQHandler         [WEAK]
                EXPORT  Reserved52_IRQHandler         [WEAK]
                EXPORT  Reserved53_IRQHandler         [WEAK]
                EXPORT  ADC_ERR_IRQHandler         [WEAK]
                EXPORT  ADCA_IRQHandler         [WEAK]
                EXPORT  CMP0_IRQHandler         [WEAK]
                EXPORT  CMP1_IRQHandler         [WEAK]
                EXPORT  FTM0_IRQHandler         [WEAK]
                EXPORT  FTM1_IRQHandler         [WEAK]
                EXPORT  Reserved60_IRQHandler         [WEAK]
                EXPORT  Reserved61_IRQHandler         [WEAK]
                EXPORT  Reserved62_IRQHandler         [WEAK]
                EXPORT  Reserved63_IRQHandler         [WEAK]
                EXPORT  PIT0_IRQHandler         [WEAK]
                EXPORT  PIT1_IRQHandler         [WEAK]
                EXPORT  PIT2_IRQHandler         [WEAK]
                EXPORT  PIT3_IRQHandler         [WEAK]
                EXPORT  PDB0_IRQHandler         [WEAK]
                EXPORT  Reserved69_IRQHandler         [WEAK]
                EXPORT  XBARA_IRQHandler         [WEAK]
                EXPORT  PDB1_IRQHandler         [WEAK]
                EXPORT  Reserved72_IRQHandler         [WEAK]
                EXPORT  MCG_IRQHandler         [WEAK]
                EXPORT  LPTMR0_IRQHandler         [WEAK]
                EXPORT  PORTA_IRQHandler         [WEAK]
                EXPORT  PORTB_IRQHandler         [WEAK]
                EXPORT  PORTC_IRQHandler         [WEAK]
                EXPORT  PORTD_IRQHandler         [WEAK]
                EXPORT  PORTE_IRQHandler         [WEAK]
                EXPORT  SWI_IRQHandler         [WEAK]
                EXPORT  Reserved81_IRQHandler         [WEAK]
                EXPORT  ENC_COMPARE_IRQHandler         [WEAK]
                EXPORT  ENC_HOME_IRQHandler         [WEAK]
                EXPORT  ENC_WDOG_SAB_IRQHandler         [WEAK]
                EXPORT  ENC_INDEX_IRQHandler         [WEAK]
                EXPORT  CMP2_IRQHandler         [WEAK]
                EXPORT  FTM3_IRQHandler         [WEAK]
                EXPORT  Reserved88_IRQHandler         [WEAK]
                EXPORT  ADCB_IRQHandler         [WEAK]
                EXPORT  Reserved90_IRQHandler         [WEAK]
                EXPORT  CAN0_ORed_Message_buffer_IRQHandler         [WEAK]
                EXPORT  CAN0_Bus_Off_IRQHandler         [WEAK]
                EXPORT  CAN0_Error_IRQHandler         [WEAK]
                EXPORT  CAN0_Tx_Warning_IRQHandler         [WEAK]
                EXPORT  CAN0_Rx_Warning_IRQHandler         [WEAK]
                EXPORT  CAN0_Wake_Up_IRQHandler         [WEAK]
                EXPORT  Reserved97_IRQHandler         [WEAK]
                EXPORT  Reserved98_IRQHandler         [WEAK]
                EXPORT  Reserved99_IRQHandler         [WEAK]
                EXPORT  Reserved100_IRQHandler         [WEAK]
                EXPORT  Reserved101_IRQHandler         [WEAK]
                EXPORT  Reserved102_IRQHandler         [WEAK]
                EXPORT  Reserved103_IRQHandler         [WEAK]
                EXPORT  Reserved104_IRQHandler         [WEAK]
                EXPORT  Reserved105_IRQHandler         [WEAK]
                EXPORT  Reserved106_IRQHandler         [WEAK]
                EXPORT  Reserved107_IRQHandler         [WEAK]
                EXPORT  CMP3_IRQHandler         [WEAK]
                EXPORT  Reserved109_IRQHandler         [WEAK]
                EXPORT  CAN1_ORed_Message_buffer_IRQHandler         [WEAK]
                EXPORT  CAN1_Bus_Off_IRQHandler         [WEAK]
                EXPORT  CAN1_Error_IRQHandler         [WEAK]
                EXPORT  CAN1_Tx_Warning_IRQHandler         [WEAK]
                EXPORT  CAN1_Rx_Warning_IRQHandler         [WEAK]
                EXPORT  CAN1_Wake_Up_IRQHandler         [WEAK]
                EXPORT  DefaultISR         [WEAK]
DMA0_IRQHandler
DMA1_IRQHandler
DMA2_IRQHandler
DMA3_IRQHandler
DMA4_IRQHandler
DMA5_IRQHandler
DMA6_IRQHandler
DMA7_IRQHandler
DMA8_IRQHandler
DMA9_IRQHandler
DMA10_IRQHandler
DMA11_IRQHandler
DMA12_IRQHandler
DMA13_IRQHandler
DMA14_IRQHandler
DMA15_IRQHandler
DMA_Error_IRQHandler
MCM_IRQHandler
FTFA_IRQHandler
FTFA_Collision_IRQHandler
PMC_IRQHandler
LLWU_IRQHandler
WDOG_EWM_IRQHandler
Reserved39_IRQHandler
I2C0_IRQHandler
Reserved41_IRQHandler
SPI_IRQHandler
Reserved43_IRQHandler
Reserved44_IRQHandler
Reserved45_IRQHandler
Reserved46_IRQHandler
UART0_RX_TX_IRQHandler
UART0_ERR_IRQHandler
UART1_RX_TX_IRQHandler
UART1_ERR_IRQHandler
Reserved51_IRQHandler
Reserved52_IRQHandler
Reserved53_IRQHandler
ADC_ERR_IRQHandler
ADCA_IRQHandler
CMP0_IRQHandler
CMP1_IRQHandler
FTM0_IRQHandler
FTM1_IRQHandler
Reserved60_IRQHandler
Reserved61_IRQHandler
Reserved62_IRQHandler
Reserved63_IRQHandler
PIT0_IRQHandler
PIT1_IRQHandler
PIT2_IRQHandler
PIT3_IRQHandler
PDB0_IRQHandler
Reserved69_IRQHandler
XBARA_IRQHandler
PDB1_IRQHandler
Reserved72_IRQHandler
MCG_IRQHandler
LPTMR0_IRQHandler
PORTA_IRQHandler
PORTB_IRQHandler
PORTC_IRQHandler
PORTD_IRQHandler
PORTE_IRQHandler
SWI_IRQHandler
Reserved81_IRQHandler
ENC_COMPARE_IRQHandler
ENC_HOME_IRQHandler
ENC_WDOG_SAB_IRQHandler
ENC_INDEX_IRQHandler
CMP2_IRQHandler
FTM3_IRQHandler
Reserved88_IRQHandler
ADCB_IRQHandler
Reserved90_IRQHandler
CAN0_ORed_Message_buffer_IRQHandler
CAN0_Bus_Off_IRQHandler
CAN0_Error_IRQHandler
CAN0_Tx_Warning_IRQHandler
CAN0_Rx_Warning_IRQHandler
CAN0_Wake_Up_IRQHandler
Reserved97_IRQHandler
Reserved98_IRQHandler
Reserved99_IRQHandler
Reserved100_IRQHandler
Reserved101_IRQHandler
Reserved102_IRQHandler
Reserved103_IRQHandler
Reserved104_IRQHandler
Reserved105_IRQHandler
Reserved106_IRQHandler
Reserved107_IRQHandler
CMP3_IRQHandler
Reserved109_IRQHandler
CAN1_ORed_Message_buffer_IRQHandler
CAN1_Bus_Off_IRQHandler
CAN1_Error_IRQHandler
CAN1_Tx_Warning_IRQHandler
CAN1_Rx_Warning_IRQHandler
CAN1_Wake_Up_IRQHandler
DefaultISR
                B      DefaultISR
                ENDP
                  ALIGN


                END
