SNMP Example

1. Introduction
---------------
The document explains the Snmp example, what to expect from the example and a
brief introduction to the API.
The example code demonstrates how SNMP (Simple Network Management Protocol)
uses Trap command message to communicate between the manager and the agent.
The example initializes MIB (Management Information Base), MIB demo table,
SNMP agent with traps and then performs several operations such as
send/add/remove.
The example also provides a SNMP demo that uses SNMP trap to change the
counter value through the SNMP protocol.

2. Example configuration
------------------------
All configuration options for SNMP example are stored in file config.h. All
configurations are done using macros. These options are:
  ENET_IPADDR - Board IP address for IPv4 protocol. Default is 192.168.1.202.
  ENET_IPMASK – IP mask for IPv4. Default is 255.255.255.0.
  ENET_TRAP_ADDR1 – Trap receiver address. Default is 192.168.1.205.
  ENET_TRAP_ADDR2 – Trap receiver address. Default is 192.168.1.206.
  ENET_TRAP_ADDR3 – Trap receiver address. Default is 192.168.1.207.
  ENET_TRAP_ADDR4 – Trap receiver address. Default is 192.168.1.208.
  ENET_TRAP_ADDR5 – Trap receiver address. Default is 192.168.1.209.
  ENET_ENETADDR – The target’s Ethernet address. Default is {0x00, 0x00, 0x5E, _IPBN(2), _IPBN(1), _IPBN(0)}
  ENET_IPGATEWAY – Gateway for IPV4 protocol. Default is 0.0.0.0.

Check that the RTCSCFG_ENABLE_IP4, RTCSCFG_ENABLE_SNMP and MQX_HAS_TIME_SLICE
macros are set to 1 in the file user_config.h. Then rebuild the BSP, PSP,
RTCS and SHELL projects for the target platform/IDE.

3. Running the example
----------------------
Connect a serial cable from the TWR-SER to the PC. Connect an Ethernet cable
from the RJ45 (Ethernet) connector from the board to the RJ45 connector in
the PC.
Install “iReasoning Mib Browser” on your PC.
Start a terminal application on your PC and set the serial connection for
115200 baud, 8 data bits, 1 stop bit, no parity and no flow control.
Open “iReasoning Mib Browser” and configure for it:
- Address: is IP address of the board (see in \snmp\config.h)
- OID: ".1.3.6.1.4.1.33118.1.1.1.3.0"
At "Operation" field, select "set" value. On "SNMP SET" window, select
"Integer" value for "data type" field and at "Value" field, type a number,
click OK and view result on terminal.
The counter value will be updated according to numeric value input via the
iReasoning Mib Browser.

4. To use WIFI on snmp example
------------------------------
Please follow steps below:
    1. By default, WIFI is disabled. To use WIFI, BSPCFG_ENABLE_<vendor>_WIFI definition must be enabled in user_config.h
    2. Build required libraries.
    3. Update WIFI configuration in config.h header file if necessary.
    4. Build example then run.

5. Automated generation of MIB file
-----------------------------------
The snmp_demo.c and snmp_demo.mib files were generated using the "gawk" tool according to 
parameters from the snmp_demo.def file.
After any change in snmp_demo.def you should run the def2c_def2mib.bat batch file.
The "gawk" tool is not distributed with the RTCS installation, but it can be downloaded from 
the following link: https://gnu-on-windows.googlecode.com/files/gawk-4.1.0-bin.zip 
and placed to the right "/tools" directory.

