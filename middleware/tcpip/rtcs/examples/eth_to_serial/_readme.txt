                    Ethernet to serial example application

1. Introduction
---------------
The eth_to_serial example code demonstrates a simple character passing between 
UART console and socket. The application creates a listening socket on port 23 
(this port is default and can be changed with demo configuration) to which a 
TCP/IP connection can be established. This example supports both IPv4 and IPv6 
protocols.

1.1 Content of example directory
--------------------------------
Example directory consist of following folders and files:
  - _readme.txt: This README file.
  - demo.c: Source code of Ethernet to serial example.
  - config.h: Configuration file of the example.

2. Application configuration
----------------------------
This chapter contains description of all configuration options available in the 
example.

2.1 Default Example Configuration
---------------------------------
All example configuration is done in the file "config.h" in the example folder.
By default following configuration is used:

  - Board IPv4 address: 192.168.1.202/24.
  - Board IPv6 addresses (autoconfigured): link local address.
  - IPv6 scope ID: 0.
  - Listening address family: all families enabled in the RTCS.

2.2 Configuration macros
------------------------
Following is list of configuration macros in file "config.h"

 - Server IP address is configurable with macro ENET_IPADDR.
 - Macro ENET_IPMASK defines IPv4 network mask for all interfaces.
 - Macro DEMO_PORT defines port on which demo will listen for incoming connections.
 - Macro ENET_MAC defines MAC address assigned to interface.
 - Macro SERIAL_DEVICE defines which device will be connected to through socket.

2.3 To use WIFI on eth_to_serial example
----------------------------------------
Please follow steps below:
1.	By default, WIFI is disabled. To use WIFI, BSPCFG_ENABLE_<vendor>_WIFI definition must be enabled in user_config.h
2.	Build required libraries.
3.	Update WIFI configuration in config.h header file if necessary.
4.	Build example then run.

3. Running the application
--------------------------
1) Prepare your evaluation board according to the "MQX Getting started" document.
   Pay special attention to correct jumper settings.
2) Connect your PC to the evaluation board using a serial cable.
3) Connect the evaluation board and your PC to same Ethernet network.
4) Start Terminal application on your PC. 
   Set serial port settings to 115200 baud rate, no parity and 8 bits.
5) Modify "user_config.h" so the board uses interrupted serial device as 
   a standard I/O device. Following is example for TWRK60N512/TWRK60F120M:
    #define BSPCFG_ENABLE_ITTYF 1
    #define BSP_DEFAULT_IO_CHANNEL_DEFINED
    #define BSP_DEFAULT_IO_CHANNEL "ittyf:"
6) Compile all required libraries (BSP, PSP, RTCS).
7) Compile, load and run the application. Following message should show up in 
   the terminal window:  
    Application listening on following ip addresses: 
    IPv4 Address: 192.168.1.202, port:23

    Waiting for incoming connection...
8) Application is now running and listening for connections.
9) Connect TCP terminal program in your PC (i.e NetCat or PuTTy) to address and 
   port displayed in step 6.
10) If connection was successful following text is send to UART:
    Serial <-> Ethernet bridge
    And this text in TCP terminal:
    Ethernet <-> Serial bridge
11) The TCP connection to board is established. Now all characters you input to 
    UART are echoed to TCP terminal and vice versa.
    NOTE: Please be aware that most TCP terminal applications send data only 
    after you press the enter key (send a newline sequence).
12) If you abort TCP connection to board following message is send to UART:
    Client disconnected.
    Waiting for incoming connection...
    After this message you can repeat the connection from step 9.
