################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../source/ADC_task.cpp \
../source/CAN_command.cpp \
../source/CAN_sniffer.cpp \
../source/FTDI_emulator.cpp \
../source/GPIs.cpp \
../source/GPOs.cpp \
../source/I2C_monitor.cpp \
../source/I2C_scripter.cpp \
../source/I2C_search.cpp \
../source/I2C_sniffer.cpp \
../source/I2C_task.cpp \
../source/PWM_page.cpp \
../source/SPI_command.cpp \
../source/SPI_monitor.cpp \
../source/SPI_sniffer.cpp \
../source/UART_command.cpp \
../source/UART_streamer.cpp \
../source/UART_to_USB.cpp \
../source/console_task.cpp \
../source/console_uart.cpp \
../source/error_logger.cpp \
../source/initialization.cpp \
../source/io_control_page.cpp \
../source/main.cpp \
../source/memory_functions.cpp \
../source/menu_page.cpp \
../source/neopixel_command.cpp \
../source/neopixel_menu.cpp \
../source/neopixel_rgb.cpp \
../source/number_converter.cpp \
../source/one_wire_command.cpp \
../source/one_wire_search.cpp \
../source/task_manager.cpp \
../source/test_task.cpp \
../source/text_controls.cpp \
../source/utils.cpp 

CMD_SRCS += \
../source/tm4c123gh6pm.cmd 

C_SRCS += \
../source/heap_4.c \
../source/hooks.c \
../source/tm4c123gh6pm_startup_ccs.c \
../source/usb_serial_structs.c 

C_DEPS += \
./source/heap_4.d \
./source/hooks.d \
./source/tm4c123gh6pm_startup_ccs.d \
./source/usb_serial_structs.d 

OBJS += \
./source/ADC_task.obj \
./source/CAN_command.obj \
./source/CAN_sniffer.obj \
./source/FTDI_emulator.obj \
./source/GPIs.obj \
./source/GPOs.obj \
./source/I2C_monitor.obj \
./source/I2C_scripter.obj \
./source/I2C_search.obj \
./source/I2C_sniffer.obj \
./source/I2C_task.obj \
./source/PWM_page.obj \
./source/SPI_command.obj \
./source/SPI_monitor.obj \
./source/SPI_sniffer.obj \
./source/UART_command.obj \
./source/UART_streamer.obj \
./source/UART_to_USB.obj \
./source/console_task.obj \
./source/console_uart.obj \
./source/error_logger.obj \
./source/heap_4.obj \
./source/hooks.obj \
./source/initialization.obj \
./source/io_control_page.obj \
./source/main.obj \
./source/memory_functions.obj \
./source/menu_page.obj \
./source/neopixel_command.obj \
./source/neopixel_menu.obj \
./source/neopixel_rgb.obj \
./source/number_converter.obj \
./source/one_wire_command.obj \
./source/one_wire_search.obj \
./source/task_manager.obj \
./source/test_task.obj \
./source/text_controls.obj \
./source/tm4c123gh6pm_startup_ccs.obj \
./source/usb_serial_structs.obj \
./source/utils.obj 

CPP_DEPS += \
./source/ADC_task.d \
./source/CAN_command.d \
./source/CAN_sniffer.d \
./source/FTDI_emulator.d \
./source/GPIs.d \
./source/GPOs.d \
./source/I2C_monitor.d \
./source/I2C_scripter.d \
./source/I2C_search.d \
./source/I2C_sniffer.d \
./source/I2C_task.d \
./source/PWM_page.d \
./source/SPI_command.d \
./source/SPI_monitor.d \
./source/SPI_sniffer.d \
./source/UART_command.d \
./source/UART_streamer.d \
./source/UART_to_USB.d \
./source/console_task.d \
./source/console_uart.d \
./source/error_logger.d \
./source/initialization.d \
./source/io_control_page.d \
./source/main.d \
./source/memory_functions.d \
./source/menu_page.d \
./source/neopixel_command.d \
./source/neopixel_menu.d \
./source/neopixel_rgb.d \
./source/number_converter.d \
./source/one_wire_command.d \
./source/one_wire_search.d \
./source/task_manager.d \
./source/test_task.d \
./source/text_controls.d \
./source/utils.d 

OBJS__QUOTED += \
"source\ADC_task.obj" \
"source\CAN_command.obj" \
"source\CAN_sniffer.obj" \
"source\FTDI_emulator.obj" \
"source\GPIs.obj" \
"source\GPOs.obj" \
"source\I2C_monitor.obj" \
"source\I2C_scripter.obj" \
"source\I2C_search.obj" \
"source\I2C_sniffer.obj" \
"source\I2C_task.obj" \
"source\PWM_page.obj" \
"source\SPI_command.obj" \
"source\SPI_monitor.obj" \
"source\SPI_sniffer.obj" \
"source\UART_command.obj" \
"source\UART_streamer.obj" \
"source\UART_to_USB.obj" \
"source\console_task.obj" \
"source\console_uart.obj" \
"source\error_logger.obj" \
"source\heap_4.obj" \
"source\hooks.obj" \
"source\initialization.obj" \
"source\io_control_page.obj" \
"source\main.obj" \
"source\memory_functions.obj" \
"source\menu_page.obj" \
"source\neopixel_command.obj" \
"source\neopixel_menu.obj" \
"source\neopixel_rgb.obj" \
"source\number_converter.obj" \
"source\one_wire_command.obj" \
"source\one_wire_search.obj" \
"source\task_manager.obj" \
"source\test_task.obj" \
"source\text_controls.obj" \
"source\tm4c123gh6pm_startup_ccs.obj" \
"source\usb_serial_structs.obj" \
"source\utils.obj" 

C_DEPS__QUOTED += \
"source\heap_4.d" \
"source\hooks.d" \
"source\tm4c123gh6pm_startup_ccs.d" \
"source\usb_serial_structs.d" 

CPP_DEPS__QUOTED += \
"source\ADC_task.d" \
"source\CAN_command.d" \
"source\CAN_sniffer.d" \
"source\FTDI_emulator.d" \
"source\GPIs.d" \
"source\GPOs.d" \
"source\I2C_monitor.d" \
"source\I2C_scripter.d" \
"source\I2C_search.d" \
"source\I2C_sniffer.d" \
"source\I2C_task.d" \
"source\PWM_page.d" \
"source\SPI_command.d" \
"source\SPI_monitor.d" \
"source\SPI_sniffer.d" \
"source\UART_command.d" \
"source\UART_streamer.d" \
"source\UART_to_USB.d" \
"source\console_task.d" \
"source\console_uart.d" \
"source\error_logger.d" \
"source\initialization.d" \
"source\io_control_page.d" \
"source\main.d" \
"source\memory_functions.d" \
"source\menu_page.d" \
"source\neopixel_command.d" \
"source\neopixel_menu.d" \
"source\neopixel_rgb.d" \
"source\number_converter.d" \
"source\one_wire_command.d" \
"source\one_wire_search.d" \
"source\task_manager.d" \
"source\test_task.d" \
"source\text_controls.d" \
"source\utils.d" 

CPP_SRCS__QUOTED += \
"../source/ADC_task.cpp" \
"../source/CAN_command.cpp" \
"../source/CAN_sniffer.cpp" \
"../source/FTDI_emulator.cpp" \
"../source/GPIs.cpp" \
"../source/GPOs.cpp" \
"../source/I2C_monitor.cpp" \
"../source/I2C_scripter.cpp" \
"../source/I2C_search.cpp" \
"../source/I2C_sniffer.cpp" \
"../source/I2C_task.cpp" \
"../source/PWM_page.cpp" \
"../source/SPI_command.cpp" \
"../source/SPI_monitor.cpp" \
"../source/SPI_sniffer.cpp" \
"../source/UART_command.cpp" \
"../source/UART_streamer.cpp" \
"../source/UART_to_USB.cpp" \
"../source/console_task.cpp" \
"../source/console_uart.cpp" \
"../source/error_logger.cpp" \
"../source/initialization.cpp" \
"../source/io_control_page.cpp" \
"../source/main.cpp" \
"../source/memory_functions.cpp" \
"../source/menu_page.cpp" \
"../source/neopixel_command.cpp" \
"../source/neopixel_menu.cpp" \
"../source/neopixel_rgb.cpp" \
"../source/number_converter.cpp" \
"../source/one_wire_command.cpp" \
"../source/one_wire_search.cpp" \
"../source/task_manager.cpp" \
"../source/test_task.cpp" \
"../source/text_controls.cpp" \
"../source/utils.cpp" 

C_SRCS__QUOTED += \
"../source/heap_4.c" \
"../source/hooks.c" \
"../source/tm4c123gh6pm_startup_ccs.c" \
"../source/usb_serial_structs.c" 


