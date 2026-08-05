# CC35XX Matter v1.5 Getting Started Guide

## Prerequisites

**Hardware:**
- LP-EM-CC35X1 Rev A LaunchPad
- XDS110 Programmer/Debugger

**Software:**
- Ubuntu 22.04 (host machine)
- SimpleLink Wi-Fi SDK `10.10.01.08`
- SysConfig `1.26.3`
- SimpleLink Wi-Fi Toolbox `4.2.4`
- cmake `3.21.3`
- arm gnu toolchain `12.3.rel1`

> **Note:** 
>- The SimpleLink Wi-Fi SDK `10.10.01.08` is included as a git submodule and fetched via `checkout_submodules.py`
>- SysConfig and SimpleLink Wi-Fi Toolbox can be either installed separately or can be installed with SimpleLink Wi-Fi SDK `10.10.01.08` installer.

---

## Overview

This guide covers the environment setup required to build and flash a Matter example to the TI CC35XX LaunchPad.

The TI CC35XX currently supports the **lighting-app** example demonstrating the use of the OnOff cluster. The On/Off operations are emulated through the status of the red LED on the LaunchPad.

---

## 1. Install Build Dependencies

> **Note:** The following steps assume a Linux host running **Ubuntu 22.04**.

```bash
sudo apt-get install git gcc g++ pkg-config libssl-dev libdbus-1-dev libglib2.0-dev \
  libavahi-client-dev ninja-build python3-venv python3-dev python3-pip unzip \
  libgirepository1.0-dev libcairo2-dev libreadline-dev
```

## 2. Clone the TI Matter Repository

```bash
# Clone the TI fork of Matter repository
git clone https://github.com/TexasInstruments/matter.git
cd matter

# Checkout the latest available Matter v1.5 release tag with CC35xx support
git checkout matter-v1.5-ti-1.1-ea.2

# Pull platform submodules
# The linux platform is included to support building the chip-tool and ota-provider examples on the host machine
./scripts/checkout_submodules.py --shallow --platform cc35xx linux --recursive

# Bootstrap the build environment (one-time setup, may take several minutes)
source ./scripts/bootstrap.sh
```

> **Note:** After the one-time bootstrap, activate the environment in every new shell before building:
> ```bash
> source ./scripts/activate.sh
> ```

## 3. Setup the SimpleLink Wi-Fi SDK

1. Install cmake and arm gnu toolchain if not present.

2. Navigate to the SimpleLink Wi-Fi SDK submodule directory:

```bash
cd third_party/ti_simplelink_sdk/repo_cc35xx
```
3. Set appropriate paths to SYSCONFIG_TOOL, SIMPLELINK_WIFI_TOOLBOX_INSTALL_DIR, CMAKE, GCC_ARMCOMPILER in **imports.mak** file in the SDK root.

4. Run the following command to build library archives needed for building the Matter application:

```bash
make GENERATOR=Ninja build-gcc
```

---

## Next Steps

Once the environment is set up, refer to the [lighting-app README](../../../examples/lighting-app/ti/cc35xx/README.md) for:
- Building the lighting example
- Flashing the device
- Commissioning and controlling the light with CHIP Tool

For additional environment setup or build related documentation, see the [Matter Building Guide](https://project-chip.github.io/connectedhomeip-doc/guides/BUILDING.html).

---
