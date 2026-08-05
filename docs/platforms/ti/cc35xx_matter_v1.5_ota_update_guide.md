# CC35XX Matter v1.5 OTA Update Guide

This guide describes the process for performing over-the-air (OTA) firmware updates on the CC35XX device running the Matter v1.5 example application.

## Prerequisites

- CC35XX lighting device already commissioned and connected to the Matter network
- Matter commissioner (chip-tool) set up
- Linux host machine for building the OTA provider and Matter application upgrade image for CC35xx

> **Note:** OTA is enabled by default in the CC35XX lighting app build. The `chip_enable_ota_requestor = true` setting in `examples/lighting-app/ti/cc35xx/args.gni` allows the device to request and apply OTA updates. This does not need to be modified unless OTA functionality is intentionally disabled.

## Overview

The OTA update process involves:

1. Building an OTA provider application
2. Creating an OTA-formatted upgrade image from the lighting app
3. Configuring and starting the OTA provider
4. Commissioning the OTA provider to the Matter network
5. Setting up access control lists (ACL) to allow the device to fetch from the provider
6. Triggering the OTA update on the device

## Step 1: Build the OTA Provider

The OTA provider application serves firmware images to devices requesting updates.

```bash
cd {matter-root}
source ./scripts/activate.sh

scripts/examples/gn_build_example.sh examples/ota-provider-app/linux <output-path> chip_config_network_layer_ble=false
```

**Parameters:**
- `<output-path>`: Desired output directory (e.g., `out/debug`). You can specify any path.

The OTA provider will be built at `<output-path>/chip-ota-provider-app`.

## Step 2: Build the OTA Upgrade Image

Build a new version of the lighting application with incremented version numbers. This will be packaged as an OTA image.

```bash
cd {matter-root}/examples/lighting-app/ti/cc35xx

gn gen <output-path> --args="ti_sysconfig_root=\"$HOME/ti/sysconfig_1.26.3\" ti_simplelink_wifi_toolbox_root=\"$HOME/ti/simplelink_wifi_toolbox_lin_4_2_4\" chip_generate_link_map_file=true matter_software_ver=\"0x00020000\" matter_software_ver_str=\"0.2.0\"" --export-compile-commands

ninja -C <output-path>
```

**Parameters:**
- `<output-path>`: Desired output directory (e.g., `out/debug`). You can specify any path.
- `matter_software_ver`: Version in hex format (e.g., `0x00020000` for v0.2.0)
- `matter_software_ver_str`: Human-readable version string

The OTA image will be generated at `<output-path>/chip-LP_EM_CC35X1-lighting-example.ota`.

> **Important:** The `matter_software_ver` and `matter_software_ver_str` values must be **higher than the versions currently running on the device**. Check the default values in `examples/lighting-app/ti/cc35xx/args.gni` to ensure your upgrade version is greater. The device will reject OTA images with equal or lower version numbers.

## Step 3: Start the OTA Provider

Copy the OTA image to the provider location and start the OTA provider application.

```bash
cd {ota-provider-output-dir}

cp {matter-root}/examples/lighting-app/ti/cc35xx/out/debug/chip-LP_EM_CC35X1-lighting-example.ota ./

./chip-ota-provider-app --passcode 20202021 --discriminator 22 --secured-device-port 5565 --KVS /tmp/chip_kvs_provider -f chip-LP_EM_CC35X1-lighting-example.ota -q updateAvailable
```

**Parameters:**
- `--passcode`: Setup passcode for commissioning the provider (default: `20202021`)
- `--discriminator`: Discriminator for commissioning (e.g., `22`)
- `--secured-device-port`: Port for Matter communication (default: `5565`)
- `--KVS`: Key-value store path for the provider
- `-f`: Path to the OTA image file
- `-q`: Query queue format (e.g., `updateAvailable` to advertise updates)

The OTA provider will start and advertise the available update.

## Step 4: Commission the OTA Provider

Using chip-tool, commission the OTA provider to the Matter network. Use a distinct nodeID (e.g., `2`) to differentiate from the lighting device.

```bash
./chip-tool pairing onnetwork 2 20202021
```

**Parameters:**
- `2`: nodeID for the OTA provider (use any value; remember it for later steps)
- `20202021`: Setup passcode for the OTA provider

Wait for the commissioning to complete successfully.

## Step 5: Commission the CC35XX Device (if not already done)

If the CC35XX lighting device has not been commissioned yet, commission it now:

```bash
./chip-tool pairing ble-wifi 1 <wifi_ssid> <wifi_password> 20202021 3840
```

**Parameters:**
- `1`: nodeID for the lighting device
- `<wifi_ssid>`: Wi-Fi network SSID
- `<wifi_password>`: Wi-Fi network password
- `20202021`: Setup PIN
- `3840`: Discriminator

## Step 6: Install Access Control Lists (ACL)

Configure ACL entries to allow the lighting device to communicate with the OTA provider. This grants the lighting device permission to query and download from the provider.

```bash
./chip-tool accesscontrol write acl '[{"fabricIndex": 1, "privilege": 5, "authMode": 2, "subjects": [112233], "targets": null}, {"fabricIndex": 1, "privilege": 3, "authMode": 2, "subjects": null, "targets": [{"cluster": 41, "endpoint": null, "deviceType": null}]}]' 2 0
```

**Parameters:**
- `2`: nodeID of the OTA provider
- `0`: Endpoint ID
- First ACL entry: Grants admin privileges to nodeID 112233 (default controller) for all clusters and endpoints
- Second ACL entry: Grants operate privileges to all nodes for the OTA Provider cluster (0x0029) on all endpoints

## Step 7: Trigger the OTA Update

Announce the OTA provider to the lighting device, triggering it to query for updates.

```bash
./chip-tool otasoftwareupdaterequestor announce-otaprovider 2 0 0 0 1 0
```

**Parameters:**
- `2`: nodeID of the OTA provider
- `1`: nodeID of the lighting device (target)
- `0`: Endpoint IDs and other parameters for the announcement

## Monitoring the Update

Monitor the logging output for below logs on the CC35XX device (via UART at 115200 baud) to observe the OTA process:

- `Downloaded X / Y bytes` - Download progress
- `OTA image download complete (Z bytes)` - Download finished successfully
- `OTA install OK, requesting Reboot 1 (trial boot)` - Image ready, initiating trial boot
- Device reboots into **Trial Boot** - New image is tested to verify it boots
- `OTA commit OK, requesting Reboot 2 (commit boot)` - Trial boot successful, image accepted
- Device reboots into **Commit Boot** - New image is now permanent

> **Note:** The CC35XX OTA flow has two reboots. Reboot 1 (trial boot) verifies the new firmware is bootable. Only after the successful trial boot does the device commit the image. If the trial boot fails, the device automatically rolls back to the previous version on the next boot.

Check the software version of the image running on CC35xx post-update by querying the Basic Information Cluster:

```bash
./chip-tool basicinformation read software-version 1 0
```

The version should match the `matter_software_ver` value from the upgrade image.

## Troubleshooting

| Issue | Cause | Solution |
|-------|-------|----------|
| QueryImage command is denied  | ACL not properly installed on provider | Follow Step 6 and install ACL entries |
| Upgrade image is not downloaded | Image version same or degrade from current | Rebuild with higher `matter_software_ver` |
