# TI Matter Release Notes

This Texas Instruments Matter GitHub repository contains the software
development tools that enable engineers to develop Matter devices and is the
starting point for Matter development on all SimpleLink™ Thread and Wi-Fi®
devices.

## Supported Matter Devices

| Device                                            | Matter Thread | Matter Wi-Fi | Production Ready | Evaluation Only |
| ------------------------------------------------- | :-----------: | :----------: | :--------------: | :-------------: |
| [CC2674R10](https://www.ti.com/product/CC2674R10) |       x       |              |                  |        x        |
| [CC2674P10](https://www.ti.com/product/CC2674P10) |       x       |              |                  |        x        |
| [CC2745R10](https://www.ti.com/product/CC2745R10) |       x       |              |                  |        x        |
| [CC2755P10](https://www.ti.com/product/CC2755P10) |       x       |              |                  |        x        |
| [CC2755P20](https://www.ti.com/product/CC2755P20) |       x       |              |                  |        x        |
| [CC2755R10](https://www.ti.com/product/CC2755R10) |       x       |              |                  |        x        |
| [CC3551E](https://www.ti.com/product/CC3551E)     |               |      x       |                  |        x        |

## What's New

- Ported Matter 1.5 lighting app example for CC2755xx platform.
- Upgrade CC27xx SimpleLink F3 SDK to lpf3-9.20.00.81 release
- Update ot-ti dependency to thread-v1.4-ti-2.1

## Fixed Issues

- None

## Known Issues

- None

## TI Thread Certification ID

| Platform         | Configuration | Thread Spec version v1.4 | Certification |
| ---------------- | :-----------: | :----------------------: | :-----------: |
| CC27xx           |    FTD/MTD    |            --            | Pending       |

## Operating System Support

- Ubuntu v22.04

## Versioning

This product follows a version format, {Matter_Spec_Version}-{MM}.{mm}

- Matter_Spec_Version is the Matter spec version
- MM - major release version
- mm - minor release version

For EA releases an additional version qualifier "-ea-{AA}.{bb}" will be added:

- ea - Early Access
- AA - major release version
- bb - minor release version

## Dependencies

### Matter Over Thread

- TI OpenThread thread-v1.4-ti-2.1 commit id
    [eaac0deb4b](https://github.com/TexasInstruments/ot-ti/commit/eaac0deb4b3369742f43416c26531cc16938d8c6)
  - Based on Thread v1.4 with commit id
        [c9c19aa](https://github.com/openthread/openthread/commit/c9c19aa9fa5877cf1532c35a584618900e5c99c7)
- TI SimpleLink F3 SDK: lpf3-9.20.00.81 commit id
    [b0eddf0d61](https://github.com/TexasInstruments/simplelink-lowpower-f3-sdk/commit/b0eddf0d6182b22f2dfd76c10634074804124ab3)
- FreeRTOS: 202104.00
- SysConfig: 1.27.0
- GCC: Arm GNU Toolchain 12.2.MPACBTI-Rel1 12.2.1
