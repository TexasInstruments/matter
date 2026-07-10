# CC27XX Matter Getting Started Guide

## Prerequisites

**Hardware:**

- CC27XX LaunchPad (e.g., LP-EM-CC2755P20)

**Software:**

- Ubuntu 22.04 (host machine)
- SimpleLink F3 SDK `lpf3-9.20.00.81`
- SysConfig `1.27.0` or later
- cmake `3.21.3`
- arm gnu toolchain `12.2.rel1`

> **Note:** SysConfig is installed by the SimpleLink F3 SDK installer and does not need to be installed separately.

---

## Overview

This guide covers the environment setup required to build and flash a Matter example to the TI CC27XX LaunchPad.

The TI CC27XX currently supports the **lighting-app** example demonstrating the use of the OnOff cluster over Thread. The On/Off operations are emulated through the status of the red LED on the LaunchPad.

**Key features:**

- Thread-based connectivity (no WiFi)
- BLE-based commissioning support

**Supported CC27XX variants:**

- CC2745R10-Q1 (LP_EM_CC2745R10_Q1)
- CC2755P10 (LP_EM_CC2755P10)
- CC2755P20 (LP_EM_CC2755P20) - Default
- CC2755R10-BG (LP_EM_CC2755R10_BG)

For build, flash, and commissioning instructions, refer to the [lighting-app README](../../../examples/lighting-app/ti/cc27xx/README.md).

For detailed instructions on environment setup for building matter, refer to [Building guide](https://project-chip.github.io/connectedhomeip-doc/guides/BUILDING.html) from matter documentation.

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
# Clone the Matter repository with CC27XX support
git clone https://github.com/TexasInstruments/matter.git
cd matter

# Checkout the CC27XX support branch
git checkout matter-v1.5-ti

# Pull platform submodules
# The linux platform is included to support building the chip-tool and ota-provider examples on the host machine
./scripts/checkout_submodules.py --shallow --platform cc27xx linux --recursive

# Bootstrap the build environment (one-time setup, may take several minutes)
source ./scripts/bootstrap.sh
```

> **Note:** After the one-time bootstrap, activate the environment in every new shell before building:
>
> ```bash
> source ./scripts/activate.sh
> ```

## 3. Setup the SimpleLink F3 SDK

1. Install cmake, arm gnu toolchain if not present.

2. Set appropriate paths to SYSCONFIG_TOOL, CMAKE, GCC_ARMCOMPILER in imports.mak file in SDK.

3. Run below command to build library archives needed for building matter application.

```bash
make GENERATOR=Ninja build-gcc
```

---

## Next Steps

Refer to the [lighting-app README](../../../examples/lighting-app/ti/cc27xx/README.md) for:

- Selecting the target board
- Building the example
- Flashing the device
- Commissioning and controlling the light with CHIP Tool

---

## Appendix: Version Reference

| Component         | Version           |
| ----------------- | ----------------- |
| SimpleLink F3 SDK | `lpf3-9.20.00.81` |
| SysConfig         | `1.27.0` or later |
| GCC               | `12.2.rel1`       |
| JTAG interface    | XDS110            |
