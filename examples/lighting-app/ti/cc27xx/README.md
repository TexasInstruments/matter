# Matter Lighting Example Application

An example application showing the use of [Matter][matter] on the Texas
Instruments CC27xx family of Wireless MCUs.

---

- [Matter Lighting Example Application](#matter-lighting-example-application)
  - [Introduction](#introduction)
    - [Device UI](#device-ui)
  - [Building](#building)
    - [Preparation](#preparation)
    - [Compilation](#compilation)
  - [Programming](#programming)
    - [Code Composer Studio](#code-composer-studio)
    - [UniFlash](#uniflash)
  - [Viewing Logging Output](#viewing-logging-output)
  - [Running the Example](#running-the-example)
  - [TI Support](#ti-support)

---

## Introduction

The CC27xx lighting example application provides a working demonstration of a
connected light device. This uses the open-source Matter implementation and the
Texas Instruments SimpleLink™ F3 software development kit.

This example is enabled to build for the following CC27xx devices:

- CC2745R10-Q1 (LP_EM_CC2745R10_Q1)
- CC2755P10 (LP_EM_CC2755P10)
- CC2755P20 (LP_EM_CC2755P20) - Default
- CC2755R10-BG (LP_EM_CC2755R10_BG)

The lighting example is intended to serve both as a means to explore the
workings of Matter, as well as a template for creating real products based on
the Texas Instruments devices.

## Device UI

| Action                                           | Functionality                          |
| ------------------------------------------------ | -------------------------------------- |
| Left Button (`BTN-1`) Press (less than 1000 ms)  | Light is turned on                     |
| Left Button (`BTN-1`) Press (more than 1000 ms)  | Factory Reset                          |
| Right Button (`BTN-2`) Press (less than 1000 ms) | Light is turned off                    |
| Right Button (`BTN-2`) Press (more than 1000 ms) | BLE Advertisement (Enable/Disable)     |
| Red LED Solid On State                           | Light is turned on                     |
| Red LED Off State                                | Light is turned off                    |
| Green LED Blinking State                         | Identify Trigger Effect in progress    |
| Green LED Off State                              | No Identify Trigger Effect in progress |

When the device has LIT ICD functionality enabled (`chip_enable_icd_lit` set to
true in args.gni), the functionality of the short button presses changes as
described below:

| Action                                           | Functionality            |
| ------------------------------------------------ | ------------------------ |
| Left Button (`BTN-1`) Press (less than 1000 ms)  | User Active Mode Trigger |
| Right Button (`BTN-2`) Press (less than 1000 ms) | Light is toggled         |

## Building

### Preparation

Some initial setup is necessary for preparing the build environment. This
section will need to be done when migrating to new versions of the SDK. This
guide assumes that the environment is linux based, and recommends Ubuntu 20.04.

- Download and install [SysConfig][sysconfig] v1.27.0 or later.

- Run the bootstrap script to setup the build environment.
- Note, a recursive submodule checkout is required to utilize TI's Openthread
    reference commit.
- Note, in order to build the chip-tool and ota-provider examples, a recursive
    submodule checkout is required for the linux platform as seen in the command
    below.

    ```
    cd ~/connectedhomeip
    source ./scripts/bootstrap.sh
    ./scripts/checkout_submodules.py --shallow --platform cc27xx linux --recursive

    ```

### Compilation

It is necessary to activate the environment in every new shell. Then run GN and
Ninja to build the executable.

- Activate the build environment with the repository activate script.

    ```
    cd ~/connectedhomeip
    source ./scripts/activate.sh

    ```

- Run the build. By default on Linux the TI SimpleLink SDK and SysConfig are
    located in a `ti` folder in the user's home directory. For example
    `/home/username/ti/sysconfig_1.27.0`. On Windows the default directory is
    `C:\ti`.

    Build for the default board (CC2755P20):

    ```bash
    cd ~/connectedhomeip/examples/lighting-app/ti/cc27xx
    gn gen out/debug --args="ti_sysconfig_root=\"$HOME/ti/sysconfig_1.27.0\""
    ninja -C out/debug

    ```

- To build for a different CC27xx board, pass `ti_simplelink_board` as an
    argument. Available boards: `LP_EM_CC2755P20` (default),
    `LP_EM_CC2745R10_Q1`, `LP_EM_CC2755P10`, `LP_EM_CC2755R10_BG`.

    Example for CC2745R10-Q1:

    ```bash
    gn gen out/debug --args="ti_sysconfig_root=\"$HOME/ti/sysconfig_1.27.0\" ti_simplelink_board=\"LP_EM_CC2745R10_Q1\""
    ninja -C out/debug

    ```

- Additional arguments can be passed as needed:

    ```bash
    gn gen out/debug --args="ti_sysconfig_root=\"$HOME/ti/sysconfig_1.27.0\" ti_simplelink_board=\"LP_EM_CC2755P10\" target_defines=[\"TI_ATTESTATION_CREDENTIALS=1\"]"
    ```

## Programming

Loading the built image onto a LaunchPad is supported through two methods;
Uniflash and Code Composer Studio (CCS). UniFlash can be used to load the image.
Code Composer Studio can be used to load the image and debug the source code.

### Code Composer Studio

Programming with CCS will allow for a full debug environment within the IDE.
This is accomplished by creating a target connection to the XDS110 debugger and
starting a project-less debug session. The CCS IDE will attempt to find the
source files on the local machine based on the debug information embedded within
the ELF. CCS may prompt you to find the source code if the image was built on
another machine or the source code is located in a different location than is
recorded within the ELF.

Download and install [Code Composer Studio][ccs].

First open CCS and create a new workspace.

Create a target connection (sometimes called the CCXML) for your target SoC and
debugger as described in the [Manual Method][ccs_manual_method] section of the
CCS User's Guide.

Next initiate a project-less debug session as described in the [Manual
Launch][ccs_manual_launch] section of the CCS User's Guide.

CCS should switch to the debug view described in the [After
Launch][ccs_after_launch] section of the User's Guide. The SoC core will likely
be disconnected and symbols will not be loaded. Connect to the core as described
in the [Debug View][ccs_debug_view] section of the User's Guide. Once the core
is connected, use the `Load` button on the toolbar to load the ELF image.

Note that the default configuration of the CCXML uses 2-wire cJTAG instead of
the full 4-wire JTAG connection to match the default jumper configuration of the
LaunchPad.

### UniFlash

Uniflash is Texas Instrument's uniform programming tool for embedded processors.
This will allow you to erase, flash, and inspect the SoC without setting up a
debugging environment.

Download and install [UniFlash][uniflash].

First open UniFlash. Debug probes connected to the computer will usually be
displayed under the Detected Devices due to the automatic device detection
feature. If your device does not show up in this view it my be disconnected, or
you may have to create a New Configuration. If you already have a CCXML for your
SoC and debug connection you can use that in the section at the bottom. Once
your device is selected, click the `Start` button within the section to launch
the session.

Select the ELF image to load on the device with the `Browse` button. This file
is placed in the `out/debug` folder by this guide and ends with the `*.out` file
extension. For OTA enabled applications, the standalone image will instead end
with the `*-mcuboot.hex` file extension. This this is a combined image with
application and `MCUBoot` included. The flag to enable or disable the OTA
feature is determined by "chip_enable_ota_requestor" in the application's
args.gni file.

Finally click the `Load Image` button to load the executable image onto the
device. You should be able to see the log output over the XDS110 User UART.

Note that programming the device through JTAG sets the Halt-in-Boot flag and may
cause issues when performing a software reset. This flag can be reset by
power-cycling the LaunchPad.

## Viewing Logging Output

By default the log output will be sent to the Application/User UART. Open a
terminal emulator to that port to see the output with the following options:

| Parameter    | Value    |
| ------------ | -------- |
| Speed (baud) | `115200` |
| Data bits    | `8`      |
| Stop bits    | `1`      |
| Parity       | `None`   |
| Flow control | `None`   |

## Running the Example

Once a device has been flashed with this example, it can now join and operate in
an existing Matter network. The following sections assume that a Matter network
is already active, and has at least one [OpenThread Border
Router][ot_border_router_setup].

For insight into what other components are needed to run this example, please
refer to our [Matter Getting Started Guide][matter-e2e-faq].

The steps below should be followed to commission the lighting device onto the
network and control it once it has been commissioned.

**Step 0**

Set up the CHIP tool by following the instructions outlined in our [Matter
Getting Started Guide][matter-e2e-faq].

**Step 1**

Commission the light device onto the Matter network. Run the following command
on the CHIP tool:

```

./chip-tool pairing ble-thread <nodeID - e.g. 1> hex:<complete dataset from starting the OTBR> 20202021 3840

```

Interacting with the application begins by enabling BLE advertisements and then
pairing the device into a Thread network. To provision this example onto a
Matter network, the device must be discoverable over Bluetooth LE.

On the LaunchPad, press and hold the right button, labeled `BTN-2`, for more
than 1 second. Upon release, the Bluetooth LE advertising will begin. Once the
device is fully provisioned, BLE advertising will stop.

Once the device has been successfully commissioned, you will see the following
message on the CHIP tool output:

```

[1677648218.370754][39785:39790] CHIP:CTL: Received CommissioningComplete response, errorCode=0
[1677648218.370821][39785:39790] CHIP:CTL: Successfully finished commissioning step 'SendComplete'

```

An accompanying message will be seen from the device:

```

Commissioning complete, notify platform driver to persist network credentials.

```

**Step 2**

Send commands to the lighting app. Here are some example commands:

On

```
./chip-tool onoff on <nodeID> 1
./chip-tool onoff toggle <nodeID> 1 (assuming the light is off)

```

Off

```
./chip-tool onoff off <nodeID> 1
./chip-tool onoff toggle <nodeID> 1 (assuming the light is on)

```

Identify

```
./chip-tool identify identify <IdentifyTime> <nodeID> 1   e.g.  ./chip-tool identify identify 100 1 1

./chip-tool identify trigger-effect <EffectIdentifier> <EffectVariant> <nodeID> <endpointID>  e.g.  ./chip-tool identify trigger-effect 0x0000 0 1 1

```

## TI Support

For technical support, please consider creating a post on TI's [E2E forum][e2e].
Additionally, we welcome any feedback.

[matter]: https://csa-iot.org/all-solutions/matter/
[ccs]: https://www.ti.com/tool/CCSTUDIO
[ccs_after_launch]:
    https://software-dl.ti.com/ccs/esd/documents/users_guide/ccs_debug-main.html?configuration#after-launch
[ccs_debug_view]:
    https://software-dl.ti.com/ccs/esd/documents/users_guide/ccs_debug-main.html?configuration#debug-view
[ccs_manual_launch]:
    https://software-dl.ti.com/ccs/esd/documents/users_guide/ccs_debug-main.html?configuration#manual-launch
[ccs_manual_method]:
    https://software-dl.ti.com/ccs/esd/documents/users_guide/ccs_debug-main.html?configuration#manual-method
[e2e]:
    https://e2e.ti.com/support/wireless-connectivity/zigbee-thread-group/zigbee-and-thread/f/zigbee-thread-forum
[matter-e2e-faq]:
    https://e2e.ti.com/support/wireless-connectivity/zigbee-thread-group/zigbee-and-thread/f/zigbee-thread-forum/1082428/faq-cc2652r7-matter----getting-started-guide
[sysconfig]: https://www.ti.com/tool/SYSCONFIG
[ti_thread_dnd]:
    https://www.ti.com/wireless-connectivity/thread/design-development.html
[ot_border_router_setup]: https://openthread.io/guides/border-router/build
[uniflash]: https://www.ti.com/tool/download/UNIFLASH
