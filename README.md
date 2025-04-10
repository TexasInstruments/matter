# Texas Instruments Support for Matter

![TI Logo](docs/images/ti_logo.png)

The Texas Instruments Matter GitHub repository contains the software development
tools that enable engineers to develop Matter Device and is the starting point
for Matter development on all SimpleLink™ Thread and Wi-Fi® devices. TI's matter
solution is based on the following key software components:

-   TI SimpleLink™ SDK
    -   Bluetooth® Low Energy (Bluetooth LE) protocol stack
    -   Dynamic Multi-protocol Manager
    -   Drivers
    -   FreeRTOS
-   Thread networking stack based on OpenThread
-   Matter application layer stack based on open source Matter stack

In addition this repo also contains documentation, examples, build scripts and
device support. For more details on TI technology for the Matter standard, refer
to our
[Matter overview](https://www.ti.com/technologies/wired-wireless-connectivity/matter/overview.html).

## What is Matter?

Matter is a unified, open-source application-layer connectivity standard built
to enable developers and device manufacturers to build reliable and secure
ecosystems; along with increasing compatibility among connected home devices. It
is built with market-proven technologies using Internet Protocol (IP) and is
compatible with Thread and Wi-Fi network transports. Matter was developed by a
Working Group within the Connectivity Standards Alliance.

To learn more about Matter SW visit the
[Matter GitHub](https://github.com/project-chip/connectedhomeip).

To learn more about Matter standard and how to build Matter based products visit
[buildwithmatter.com](buildwithmatter.com).

## Documentation

To start development with Matter, refer to the following:

-   Get started with Matter over Thread,
    [Matter Thread getting started](https://dev.ti.com/tirex/explore/node?a=AfkT0vQ__5.30.00.00&node=A__AciOYyNq9gli.nsvJzBtQg__com.ti.SIMPLELINK_ACADEMY_CC13XX_CC26XX_SDK__AfkT0vQ__LATEST)

-   Get started with Matter over Wi-Fi,
    [Matter Wi-Fi getting started](https://e2e.ti.com/support/wireless-connectivity/wi-fi-group/wifi/f/wi-fi-forum/1122413/faq-cc3235sf-matter----getting-started-guide)
    -   Matter Over Wi-Fi in this release is for evaluation purposes only (and
        not recommended for production).

Additional documentation is hosted in the `./docs/guides/ti folder`. It is
recommended to read the documentation in the following order:

Start off with `ti-matter-release-notes` for important information about
versioning, known issues, and more.

-   [ti-matter-release-notes](./docs/guides/ti/ti-matter-release-notes.md)

**Optional**, if migration is needed, locate the `matter-migration-guide` folder
and reference:

-   [matter_cc2674_migration](./docs/guides/ti/matter-migration-guide/matter_cc2674_migration.md)

Then navigate to the `matter-users-guide` folder and read in the following
order:

-   [ti_matter_overview](./docs/guides/ti/matter-users-guide/ti_matter_overview.md)
-   [ti_matter_product_certification](./docs/guides/ti/matter-users-guide/ti_matter_product_certification.md)
-   [ti_factory_data_user_guide](./docs/guides/ti/matter-users-guide/ti_factory_data_user_guide.md)
-   [enabling_icd_on_ti_devices](./docs/guides/ti/matter-users-guide/enabling_icd_on_ti_devices.md)

Conclude by opening the `matter-syscfg` folder and refer to the
`getting-started` guide and `sysconfig-board` as needed.

-   [getting-started](./docs/guides/ti/matter-syscfg/getting-started.md)
-   [sysconfig-board](./docs/guides/ti/matter-syscfg/sysconfig-board.md)

## License

Matter is released under the [Apache 2.0 license](./LICENSE).

Please only use the Matter name and marks when accurately referencing this
software distribution. Do not use the marks in a way that suggests you are
endorsed by or otherwise affiliated with Nest, Google, or Texas Instruments.

## Need help?

-   For technical support with Matter over Thread, including bugs and feature
    requests - submit a ticket to
    [TI's Thread E2E forum](https://e2e.ti.com/support/wireless-connectivity/zigbee-and-thread)
-   For technical support with Matter over Wi-Fi, including bugs and feature
    requests - submit a ticket to
    [TI's Wi-Fi E2E forum](https://e2e.ti.com/support/wireless-connectivity/wi-fi-group/wifi/f/wi-fi-forum)
-   Matter bugs and feature requests — submit to the
    [Matter Issue Tracker](https://github.com/project-chip/connectedhomeip/issues)

Additionally, we welcome any feedback that you can give to improve the
documentation!
