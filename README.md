# creator_blinds_contiki
This is a firmware for 6lowpan clicker which is part of the [Creator Ci40 IoT Developer Kit](http://creatordev.io/ci40-iot-dev-kit.html). This firmware is responsible for controlling window blinds using LWM2M.
NOTE: This is only proof of concept. There're no proper error handling / parameters validation etc.

## Install
```sh
git clone https://github.com/mkmk88/creator_blinds_contiki.git
cd creator_blinds_contiki
./install.sh
```
## Configure
See [project-conf.h](https://github.com/mkmk88/creator_blinds_contiki/blob/master/project-conf.h) file for different CONFIG_* options. The most important is the one that sets the IPv6 address of your LWM2M Bootstrap server.
See [Creator docs](https://docs.creatordev.io/ci40/) for information how to configure LWM2M Bootstrap server.
## Compile
You need Microchip compiler for PIC32. I recommend version [1.34](http://ww1.microchip.com/downloads/en/DeviceDoc/xc32-v1.34-full-install-linux-installer.run).
For newer compiler you might need to change version of standard library (see for instance flag --no-legacy-libc).
Run installer and remember to add the toolchain to the $PATH. After that, simply run:
```sh
make
```
## Flash
You need [MPLAB IPE](http://microchip.wikidot.com/ipe:installation) to flash. Choose board PIC32MX470F512H and select "main.hex" file for programming.
## Debug
Currently you can debug using prints over UART. It is however possible to build the project with debug symbols end export it to the [MPLAB X IDE](http://www.microchip.com/mplab/mplab-x-ide).
Detailed instruction of how to debug using MPLAB debugger is in progress.
