![image](https://github.com/bouge13a/MicroMaster/blob/main/images/aaaaaaaaaa.PNG)

# Not Another Bus Pirate...
* The MicroMaster Mini, like the Bus-Pirate, Shikra, and Hydrabus before it, is a small circuit board that communicates to a computer via a Virtual COM port.
* The purpose of the MicroMaster Mini is to allow the user to test commands that may be sent from a master to a slave device using a variety of protocols such as UART, CAN, I2C, SMBUS, SPI, 1-Wire, WS2812 (neopixel) and PWM. The MicroMaster Mini also allows the user to "sniff" CAN, SMBUS, UART, and I2C transactions.
* Unlike the Bus-Pirate and Bus-Pirate derived devices, the MicroMaster Mini employs a highly sophisticated serial interface that has a system of pages similar to a web browser that can be navigated and manipulated with an ASCII keyboard. 
* Allows users to monitor the registers of I2C devices and SPI devices in real time. 
* Has an integrated help feature for each page which describes the purpose and operation of each page. The MicroMaster Mini does not require a manual for use. 
* Allows the user to easily manipulate 4 GPOs and monitor 4 GPIs and 4 ADCs. 
* Operates as an FTDI chip emulator so that the user can transmit UART signals to a USB virtual COM port. 
* Has a suite of neopixel control tools which allow the user to easily control up to 999 neopixels!
* Programmable power supply to 1v6 3v3 and 5v. The I2C bus also level shifts with the power supply. 
* The entire project is open sourced. 
* The MicroMaster Mini is based upon the TM4C123GXL launchpad and will run on a launchpad with some minor modifications. The code is written in C++ with FreeRTOS in the Code Composer Studio IDE.

Check out the demonstration video as well as other videos on the channel. Please like and subscribe for future videos. 
https://www.youtube.com/watch?v=Cxuv18Nnr2U&t=313s

# Test and Build
The firmware will run on a TM4C123GXL launchpad. The following steps are the easiest way to test and build the MicroMaster Mini on a launchpad for testing. 

* Connect both USB ports to the host computer.
* Flash the .out file in the Debug directory to the TM4C123GXL using LMFlash (found on TI website)
* Set up a virtual COM port with the TM4C using Putty
* Start testing the MicroMaster Mini firmware! 
* Find a bug and report an issue.

Note: Some resistors will have to be removed from the launchpad to get full functionality (R9 and R10). The I2C sniffer will not work unless these resistors are removed.

# Developers
The MicroMaster Mini is still very much under development. The purpose of making this project open sourced is to get the community involved. There is still a ton of work to do. If you have any of the following skills and would like to contribute, please email me at stephan@micromaster.io. I am very responsive, so don't hesitate to reach out. 

* C++ and FreeRTOS (expert)
* Bootloaders (OpenBLT)
* Web Design
* Testing and verification (open to all skill levels)
* Marketing
* Hardware Design
* Fundraising
