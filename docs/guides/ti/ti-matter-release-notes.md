# TI Matter matter-v1.4-ti-1.0-EA-1.0 Release Notes

This Texas Instruments Matter GitHub repository contains the software
development tools that enable engineers to develop Matter Device and is the
starting point for Matter development on all SimpleLink™ Thread and Wi-Fi®
devices.

## Supported Matter Devices

| Device                                            | Matter Thread | Matter Wi-Fi | Production Ready | Evaluation Only |
| ------------------------------------------------- | :-----------: | :----------: | :--------------: | :-------------: |
| [CC2674R10](https://www.ti.com/product/CC2674R10) |       x       |              |                  |        x        |
| [CC2674P10](https://www.ti.com/product/CC2674P10) |       x       |              |                  |        x        |
| [CC2755R10](https://www.ti.com/product/CC2755R10) |       x       |              |                  |        x        |
| [CC2755P20](https://www.ti.com/product/CC2755P20) |       x       |              |                  |        x        |
| [CC3235SF](https://www.ti.com/product/CC3235SF)   |               |      x       |                  |        x        |
| [CC3551E](https://www.ti.com/product/CC3551E)     |               |      x       |                  |        x        |
| [CC3501E](https://www.ti.com/product/CC3501E)     |               |      x       |                  |        x        |

## What's New

-   MATTER-425: Add Support for CC2755P20 Platform
-   MATTER-375: Update TI Thread dependency from v1.3 GA to v1.4 EA
-   MATTER-407: Add Support for CC35xxE Platform

## Fixed Issues

-   None

## Known Issues

-   MATTER-428: Commissioning on CC2755P20 may fail first time and needs to be
    retried when testing with Chip tool running on x86 Ubuntu 22.04

## TI Thread Certification ID

| Platform         | Configuration | Thread Spec version v1.4 |
| ---------------- | :-----------: | :----------------------: |
| CC1354 or CC2674 |    FTD/MTD    |            --            |
| CC27xx           |    FTD/MTD    |            --            |

# Operating System Support

-   Ubuntu v22.04

## Versioning

This Product follows a version format, {Matter_Spec_Version}-{MM}.{mm}

-   Matter_Spec_Version is the matter spec version
-   MM - major release version
-   mm - minor release version

For EA releases additional version qualifier "-ea-{AA}.(bb)" will be added

-   ea - Early Access
-   AA - major release version
-   bb - minor release version

# Dependencies

## Matter Over Thread

-   TI OpenThread v1.4-ti-1.0-EA-1.2 commit id
    [326aae7](https://github.com/TexasInstruments/ot-ti/commit/326aae72c29041eb64f499a3d2c875e72606e8d8)
    -   Based on Thread v1.4 with commit id
        [c9c19aa](https://github.com/openthread/openthread/commit/c9c19aa9fa5877cf1532c35a584618900e5c99c7)
-   TI Simplelink F2 SDK : lpf2-8.30.01.01 commit id
    [5b31d0a](https://github.com/TexasInstruments/simplelink-lowpower-f2-sdk/commit/5b31d0a4903351e544546e23ef3330eaa4291ceb)
-   TI SimpleLink F3 SDK: lpf3-9.20.00.10_ea
-   FreeRTOS: 202104.00
-   Sysconfig: 1.23.2
-   GCC: Arm GNU Toolchain 12.2.MPACBTI-Rel1 12.2.1

## Matter Over Wi-Fi

-   TI Simplelink Wi-Fi SDK: 9.13.00.13 EA
-   FreeRTOS: 10.5.1
-   mbedTLS: 3.6.2
-   Sysconfig: 1.23.1
-   GCC: Arm GNU Toolchain 12.2.MPACBTI-Rel1 12.2.1
-   Wi-Fi ToolBox Part of TI Simplelink Wi-Fi SDK: 3.1.12

# Resources

Reference links for Launchpad per supported device

| Device    |                            Launchpad                             |
| --------- | :--------------------------------------------------------------: |
| CC2674R10 |   [LP-EM-CC1354P10-6](https://www.ti.com/tool/LP-EM-CC1354P10)   |
| CC2674P10 |   [LP-EM-CC1354P10-6](https://www.ti.com/tool/LP-EM-CC1354P10)   |
| CC2755R10 | [LP-EM-CC2745R10-Q1](https://www.ti.com/tool/LP-EM-CC2745R10-Q1) |
| CC2755P20 |                           coming soon                            |
| CC3235SF  |  [LAUNCHXL-CC3235SF](https://www.ti.com/tool/LAUNCHXL-CC3235SF)  |
| CC3551E   |       [LP-EM-CC35X1](https://www.ti.com/tool/LP-EM-CC35X1)       |
| CC3501E   |       [LP-EM-CC35X1](https://www.ti.com/tool/LP-EM-CC35X1)       |

## Technical Support and Product Updates

-   [TI SimpleLink Solutions](https://www.ti.com/wireless-connectivity/overview.html)
-   [TI E2E Community](https://e2e.ti.com/)
-   [TI Matter Overview](https://www.ti.com/matter)

# Previous Releases

## TI Matter v1.4-1.0 Release

### What's New

-   MATTER-334: Add Support for Matter Thread light-switch Application
-   MATTER-368: Transition to non-LWIP implementation of
    GenericThreadStackManager
-   MATTER-375: Add support for Dynamic Short and Long Idle Time ICD Support
-   MATTER-386: Enable ti_examples with Matter 1.4
-   MATTER-383, MATTER-387: Simplify Building for CC1354P10-1 and CC2674\*

*   This additionally simplifies migrating between any supported TI platforms.
    Refer to supported example README (such as here) for details. Previous
    CC2674 Migration Guide is now deprecated and removed.

### Fixed Issues

-   MATTER-377: BLE advertising on CC2674x has low TX power relative to CC1354P

### Known Issues

-   None

## TI Matter v1.2-1.0 Release

### What's New

-   MATTER-276: Add support for matter v1.2 specification
-   MATTER-289: Add support to build matter application with pre-built certified
    thread libraries for simplifying matter certification process.
-   MATTER-264: Add Intermittent connected devices feature support
-   MATTER-225: Add support for ability to create programmable image build with
    unique factory data elements per device.
-   MATTER-228: Add support for genericApp, a template starting point for
    developing any matter compliant product
-   MATTER-226: Add support for CC1354P10-6 platform
-   MATTER-308: Deprecate CC2652x7 platform support
-   MATTER-286: Add support for CC2674 platform via migration guide
-   MATTER-324: Update Matter opensource component to commit id b4650b9db0f
-   MATTER-311: Update OpenThread to v1.3 commit id e7fbbcc60c2
-   MATTER-325: Add support for pulling in FreeRTOS from TI SimpleLink F2 SDK
-   MATTER-268: Add support to build TI SDK drivers at build time
-   MATTER-326: Update versions for TI SimpleLink F2 SDK to v7.40.00.77 and
    Sysconfig to v 1.18.1.

### Fixed Issues

-   MATTER-313: Fixed issue where factory reset will not occur after long left
    (BTN-1) press on the LaunchPad.

### Known Issues

-   None

## The SimpleLink™ Matter v1.00.00.10 Release Notes

### What's New

-   This update adds Matter support for TI's CC1354P10-6 Launchpad. This is the
    starting point for developing matter application on the CC135x and CC267x
    platforms.
