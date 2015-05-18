@echo off

rem convert path to backslash format
set USBROOTDIR=%1
set USBROOTDIR=%USBROOTDIR:/=\%
set OUTPUTDIR=%2
set OUTPUTDIR=%OUTPUTDIR:/=\%
set TOOL=%3
set OS=%4

rem copy common files
IF NOT EXIST %OUTPUTDIR% mkdir %OUTPUTDIR%
copy %USBROOTDIR%\usb_core\include\usb.h %OUTPUTDIR%\usb.h /Y
copy %USBROOTDIR%\usb_core\include\compiler.h %OUTPUTDIR%\compiler.h /Y
copy %USBROOTDIR%\usb_core\include\types.h %OUTPUTDIR%\types.h /Y
copy %USBROOTDIR%\usb_core\include\usb_desc.h %OUTPUTDIR%\usb_desc.h /Y
copy %USBROOTDIR%\usb_core\include\usb_error.h %OUTPUTDIR%\usb_error.h /Y
copy %USBROOTDIR%\usb_core\include\usb_misc.h %OUTPUTDIR%\usb_misc.h /Y
copy %USBROOTDIR%\usb_core\include\usb_opt.h %OUTPUTDIR%\usb_opt.h /Y
copy %USBROOTDIR%\usb_core\include\usb_types.h %OUTPUTDIR%\usb_types.h /Y
copy %USBROOTDIR%\adapter\sources\adapter.h %OUTPUTDIR%\adapter.h /Y
copy %USBROOTDIR%\adapter\sources\adapter_types.h %OUTPUTDIR%\adapter_types.h /Y
copy %USBROOTDIR%\usb_core\device\include\frdmk64f120m\usb_device_config.h %OUTPUTDIR%\usb_device_config.h /Y
copy %USBROOTDIR%\usb_core\device\include\usb_device_stack_interface.h %OUTPUTDIR%\usb_device_stack_interface.h /Y
copy %USBROOTDIR%\usb_core\device\sources\classes\include\usb_class_audio.h %OUTPUTDIR%\usb_class_audio.h /Y
copy %USBROOTDIR%\usb_core\device\sources\classes\include\usb_class_cdc.h %OUTPUTDIR%\usb_class_cdc.h /Y
copy %USBROOTDIR%\usb_core\device\sources\classes\include\usb_class.h %OUTPUTDIR%\usb_class.h /Y
copy %USBROOTDIR%\usb_core\device\sources\classes\include\usb_class_composite.h %OUTPUTDIR%\usb_class_composite.h /Y
copy %USBROOTDIR%\usb_core\device\sources\classes\include\usb_class_hid.h %OUTPUTDIR%\usb_class_hid.h /Y
copy %USBROOTDIR%\usb_core\device\sources\classes\include\usb_class_msc.h %OUTPUTDIR%\usb_class_msc.h /Y
copy %USBROOTDIR%\usb_core\device\sources\classes\include\usb_class_phdc.h %OUTPUTDIR%\usb_class_phdc.h /Y


goto tool_%TOOL%

rem cw10 files
:tool_cw10
goto os_%OS%

rem iar files
:tool_iar
goto os_%OS%

rem cw10gcc files
:tool_cw10gcc
goto os_%OS%

rem uv4 files
:tool_uv4
goto os_%OS%

rem mqx file
:os_mqx
copy %USBROOTDIR%\adapter\sources\mqx\adapter_mqx.h %OUTPUTDIR%\adapter_mqx.h /Y 
copy %USBROOTDIR%\adapter\sources\mqx\adapter_cfg.h %OUTPUTDIR%\adapter_cfg.h /Y
goto copy_end

rem bm file
:os_bm
copy %USBROOTDIR%\adapter\sources\bm\adapter_bm.h %OUTPUTDIR%\adapter_bm.h /Y 
copy %USBROOTDIR%\adapter\sources\bm\adapter_cfg.h %OUTPUTDIR%\adapter_cfg.h /Y
goto copy_end

rem sdk file
:os_sdk
copy %USBROOTDIR%\adapter\sources\sdk\adapter_sdk.h %OUTPUTDIR%\adapter_sdk.h /Y 
copy %USBROOTDIR%\adapter\sources\sdk\adapter_cfg.h %OUTPUTDIR%\adapter_cfg.h /Y
goto copy_end

:copy_end

