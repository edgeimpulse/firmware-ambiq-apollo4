# Firmware repository for Ambiq Apollo 4 (internal)

Edge Impulse firmware for Ambiq Apollo 4 blue evb (apollo4P_blue_kxr_evb | Ambiq Apollo4P Blue KXR EVB)

## Prerequisites
TODO

## Build

To build the application:
```
make -j4
```
Replace 4 with a number of parallel build processes suitable for your system.

To clean the build:
```
make clean
```

## Flash
To flash the board:
```
make deploy
```

## Debug on VsCode
> [!IMPORTANT]
> You need to install [JLink software](https://www.segger.com/downloads/jlink/) and [Cortex-Debug](https://marketplace.visualstudio.com/items?itemName=marus25.cortex-debug) extension.
To start a debug session, make sure to connect the board to J6 (JLINK USB) and press F5.

## Connect to the board
Connect an USB-C cable to APOLLO4 USB connector.

## Connect Arducam Mega
see [Wiring up an Arducam](https://github.com/AmbiqAI/neuralSPOT/tree/main/neuralspot/ns-camera#wiring-up-an-arducam)
