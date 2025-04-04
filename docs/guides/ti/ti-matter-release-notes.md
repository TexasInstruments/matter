# Release Notes for matter-v1.2-ti-1.0-EA-1.1

This Texas Instruments Matter GitHub repository contains the software
development tools that enable engineers to develop Matter Device and is the
starting point for Matter development on all SimpleLink™ Thread and Wi-Fi®
devices.

## Supported Matter Devices

| Device                                            | Matter Thread | Matter Wi-Fi |                                    Production Ready\*                                     | Evaluation Only |
| ------------------------------------------------- | :-----------: | :----------: | :---------------------------------------------------------------------------------------: | :-------------: |
| [CC2674R10](https://www.ti.com/product/CC2674R10) |       x       |              | [most recent](https://github.com/TexasInstruments/matter/releases/tag/matter-ti-v1.2-1.0) |        x        |
| [CC2674P10](https://www.ti.com/product/CC2674P10) |       x       |              | [most recent](https://github.com/TexasInstruments/matter/releases/tag/matter-ti-v1.2-1.0) |        x        |
| [CC2755R10](https://www.ti.com/product/CC2755R10) |       x       |              |                                                                                           |        x        |
| [CC3235SF](https://www.ti.com/product/CC3235SF)   |               |      x       |                                                                                           |        x        |

\*For convenience, if a previous release for a device was Production Ready, the
most recent one is linked. Any EA release should be considered "Evaluation Only"
across all devices.

The classification "Evaluation Only" means one or more of the following:

-   limitations in feature support
-   limitations in test scope

Here are more details for a given device classified as "Evaluation Only":

-   CC2755:
    -   No AppoBLE support (Application over BLE)
    -   No ICD support (Intermittently Connected Device)
    -   No OTA

## What's New

-   None

## Fixed Issues

-   MATTER-403: Enable automatic OT-TI submodule initialization for CC27xx
    platform

## Known Issues

-   None

## Versioning

This Product follows a version format,
matter-{Matter_Spec_Version}-ti-{MM}.{mm}-{EA-MM}.{EA-mm}

-   Matter_Spec_Version is the Matter spec version
-   MM - TI GA major release version
-   mm - TI GA minor release version
-   EA-MM - TI EA major release version
-   EA-mm - TI EA minor release version

# Operating System Support

-   Ubuntu v22.04

# Dependencies

-   Based on Thread v1.3.0.1 with commit id
    [8bc2504](https://github.com/openthread/openthread/tree/8bc25042ba5cde20605eec2a329c0dff575303f4)
-   F2 SDK 7.40.00.77
-   F3 SDK 8.42.00.03 EA
-   FreeRTOS: 11.1.0
-   SysConfig: 1.22.0
-   GCC: Arm GNU Toolchain 12.2.MPACBTI-Rel1 12.2.1

## Technical Support and Product Updates

-   [TI SimpleLink Solutions](https://www.ti.com/wireless-connectivity/overview.html)
-   [TI E2E Community](https://e2e.ti.com/)
-   [TI Matter Overview](https://www.ti.com/matter)
