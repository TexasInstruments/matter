# TI Matter v1.4-1.0 Release Notes

This Texas Instruments Matter GitHub repository contains the software
development tools that enable engineers to develop Matter Device and is the
starting point for Matter development on all SimpleLink™ Thread and Wi-Fi®
devices.

## Supported Matter Devices

| Device                                            | Matter Thread | Matter Wi-Fi | Production Ready | Evaluation Only |
| ------------------------------------------------- | :-----------: | :----------: | :--------------: | :-------------: |
| [CC2674R10](https://www.ti.com/product/CC2674R10) |       x       |              |        x         |                 |
| [CC2674P10](https://www.ti.com/product/CC2674P10) |       x       |              |        x         |                 |
| [CC3235SF](https://www.ti.com/product/CC3235SF)   |               |      x       |                  |        x        |

## What's New

-   MATTER-334: Add Support for Matter Thread light-switch Application
-   MATTER-368: Transition to non-LWIP implementation of GenericThreadStackManager
-   MATTER-375: Add support for Dynamic Short and Long Idle Time ICD Support
-   MATTER-386: Enable ti_examples with Matter 1.4
-   MATTER-383, MATTER-387: Simplify Building for CC1354P10-1 and CC2674*

\*  This additionally simplifies migrating between any supported TI platforms.
    Refer to supported example README (such as [here](../../../examples/lighting-app/ti/cc13x4_26x4/README.md)) for details.
    Previous CC2674 Migration Guide is now deprecated and removed. 

## Fixed Issues

-   MATTER-377: BLE advertising on CC2674x has low TX power relative to CC1354P

## Known Issues

-   None

# Operating System Support

-   Ubuntu v22.04

## Versioning

This Product follows a version format, {Matter_Spec_Version}-{MM}.{mm}

-   Matter_Spec_Version is the matter spec version
-   MM - major release version
-   mm - minor release version

# Dependencies

-   TI OpenThread v1.3-1.0 commit id
    [e7fbbcc](https://github.com/TexasInstruments/ot-ti/tree/e7fbbcc60c25d1dec3ed4d02cff9acd866091ce7)
    -   Based on Thread v1.3.0.1 with commit id
        [8bc2504](https://github.com/openthread/openthread/tree/8bc25042ba5cde20605eec2a329c0dff575303f4)
-   Bluetooth LE Stack v5.x with SimpleLink SDK version: 8.30.01.01 or
    SimpleLink SDK version/commit id
    [5b31d0a](https://github.com/TexasInstruments/simplelink-lowpower-f2-sdk/commit/5b31d0a4903351e544546e23ef3330eaa4291ceb)
-   FreeRTOS: 202104.00
-   Sysconfig: 1.22.0
-   GCC: Arm GNU Toolchain 12.2.MPACBTI-Rel1 12.2.1

## Technical Support and Product Updates

-   [TI SimpleLink Solutions](https://www.ti.com/wireless-connectivity/overview.html)
-   [TI E2E Community](https://e2e.ti.com/)
-   [TI Matter Overview](https://www.ti.com/matter)

# Previous Releases

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

-   This update adds Matter support for TI's CC1354P10-6 Launchpad. This is the starting point for developing matter application on the CC135x and CC267x platforms.


