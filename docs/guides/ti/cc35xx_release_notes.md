# CC35XX Matter Release Notes

This is an intermediary evaluation release that adds support for the CC35XX Rev A LaunchPad
and integrates the SimpleLink Wi-Fi SDK 9.22.00.15. It is based on the
`matter-v1.4-ti-1.0-EA-1.0` release.

## Supported Matter Devices

| Device                                        | Matter Wi-Fi | Evaluation Only |
| --------------------------------------------- | :----------: | :-------------: |
| [CC3551E](https://www.ti.com/product/CC3551E) |      x       |        x        |
| [CC3501E](https://www.ti.com/product/CC3501E) |      x       |        x        |

> **Note:** Matter Over Wi-Fi is for evaluation purposes only and is not recommended for production.

## What's New

-   Add support for CC35XX Rev A LaunchPad
-   Update TI SimpleLink Wi-Fi SDK dependency from 9.13.00.13 EA to 9.22.00.15
-   Switch mbedTLS dependency to prebuilt library from the SDK instead of building from source
-   Switch Matter crypto backend from mbedTLS to PSA crypto (`chip_crypto = psa`)

## Fixed Issues

-   None

## Known Issues

-   None

## Operating System Support

-   Ubuntu v22.04

## Dependencies

### Matter Over Wi-Fi

-   TI SimpleLink Wi-Fi SDK: 9.22.00.15
-   FreeRTOS: 10.5.1
-   mbedTLS: 3.6.3
-   SysConfig: 1.26.1
-   Wi-Fi Toolbox: 4.1.8
-   GCC: Arm GNU Toolchain 12.2.MPACBTI-Rel1 12.2.1

## Resources

| Device  | Launchpad |
| ------- | :-------: |
| CC3551E | [LP-EM-CC35X1 Rev A](https://www.ti.com/tool/LP-EM-CC35X1) |
| CC3501E | [LP-EM-CC35X1 Rev A](https://www.ti.com/tool/LP-EM-CC35X1) |

## Technical Support and Product Updates

-   [TI E2E Community](https://e2e.ti.com/support/wireless-connectivity/wi-fi-group/wifi/f/wi-fi-forum)
-   [TI Matter Overview](./ti_matter_overview.md)

# Previous Releases

## TI Matter matter-v1.4-ti-1.0-EA-1.0 Release

### What's New

-   MATTER-407: Add Support for CC35xxE Platform

### Fixed Issues

-   None

### Known Issues

-   None
