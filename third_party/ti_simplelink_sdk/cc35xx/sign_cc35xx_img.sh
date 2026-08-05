#!/bin/bash

# signs the Matter image so it is flashable
# arg1 is the matter image out directory
# arg2 is the simplelink toolbox path
# arg3 is the generated sysconfig file path
# arg4 is the example name
# arg5 is the simplelink sdk path
# arg6 is the matter application version string (MAJOR.MINOR.PATCH.BUILD, e.g. "1.0.1.0")

MATTER_IMG_OUT_DIR=$1
WIFI_TOOLBOX_PATH=$2
SYSCONFIG_GENERATED_FILES_PATH=$3
MATTER_IMG_NAME=$4
WIFI_SDK_PATH=$5
MATTER_APP_VERSION=$6



#Build and Sign programming instructions

$WIFI_TOOLBOX_PATH/simplelink-wifi-toolbox flash-images-builder build programming_image --flash_memory_config $SYSCONFIG_GENERATED_FILES_PATH/external_memory_configurator.json --fuses_programming_instructions $WIFI_TOOLBOX_PATH/programmer/activation_and_initial_programming/jsons/sdk_example_key/fuse_prog_inst_param.CC35XXE.json --activation_type sdk_example_key --signing_module $WIFI_TOOLBOX_PATH/flash-images-builder/signing_module/sign.py --dir_out_path $MATTER_IMG_OUT_DIR
$WIFI_TOOLBOX_PATH/simplelink-wifi-toolbox flash-images-builder sign programming_image --unsign_image $MATTER_IMG_OUT_DIR/programming_instructions_image.unsign.bin --activation_type sdk_example_key --signing_module $WIFI_TOOLBOX_PATH/flash-images-builder/signing_module/sign.py

#Build and Sign programming action request
$WIFI_TOOLBOX_PATH/simplelink-wifi-toolbox flash-images-builder build action_request --type programming --params_json $SYSCONFIG_GENERATED_FILES_PATH/action_params.json --dir_out_path $MATTER_IMG_OUT_DIR
$WIFI_TOOLBOX_PATH/simplelink-wifi-toolbox flash-images-builder sign action_request --unsign_request $MATTER_IMG_OUT_DIR/programming_action_request.unsign.bin --activation_type sdk_example_key --signing_module $WIFI_TOOLBOX_PATH/flash-images-builder/signing_module/sign.py

#Build and Sign Debug action request
$WIFI_TOOLBOX_PATH/simplelink-wifi-toolbox flash-images-builder build action_request --type debug --params_json $SYSCONFIG_GENERATED_FILES_PATH/action_params.json --dir_out_path $MATTER_IMG_OUT_DIR
$WIFI_TOOLBOX_PATH/simplelink-wifi-toolbox flash-images-builder sign action_request --unsign_request $MATTER_IMG_OUT_DIR/debug_action_request.unsign.bin --activation_type sdk_example_key --signing_module $WIFI_TOOLBOX_PATH/flash-images-builder/signing_module/sign.py

#Build and Sign Primary vendor image
$WIFI_TOOLBOX_PATH/simplelink-wifi-toolbox flash-images-builder build vendor_image --conf_bin_file $WIFI_SDK_PATH/tools/TI_Containers/cc35xx-conf.bin --vendor_out_file $MATTER_IMG_OUT_DIR/$MATTER_IMG_NAME --dir_out_path $MATTER_IMG_OUT_DIR --version "$MATTER_APP_VERSION"
$WIFI_TOOLBOX_PATH/simplelink-wifi-toolbox flash-images-builder sign vendor_image --unsign_image $MATTER_IMG_OUT_DIR/vendor_image.unsign.bin --activation_type sdk_example_key --signing_module $WIFI_TOOLBOX_PATH/flash-images-builder/signing_module/sign.py

#Build Primary WSOC image
$WIFI_TOOLBOX_PATH/simplelink-wifi-toolbox flash-images-builder build ti_wireless_fw_image --ti_wireless_fw_sign_container $WIFI_SDK_PATH/tools/TI_Containers/ti_wireless_firmware_primary.bin --dir_out_path $MATTER_IMG_OUT_DIR --output_file_name "primary_ti_wireless_fw_image.bin"

#Build Boot_sector image
$WIFI_TOOLBOX_PATH/simplelink-wifi-toolbox flash-images-builder build boot_sector_image --programming_instruction_image_path $MATTER_IMG_OUT_DIR/programming_instructions_image.sign.bin --dir_out_path $MATTER_IMG_OUT_DIR 

#Build Primary TI Bootloader image
# TODO: [MATTER-454] Revert workaround to use bootloader container from Toolbox instead of SDK (bootloader container in 10.10.01.08 SDK has corrupted signature)
$WIFI_TOOLBOX_PATH/simplelink-wifi-toolbox flash-images-builder build tbl_image --ti_sign_container $WIFI_TOOLBOX_PATH/programmer/tbl/ti_bootloader.CC35XXE.sign.bin --dir_out_path $MATTER_IMG_OUT_DIR --output_file_name "primary_tbl_image.bin"

#Build tool_settings.json
$WIFI_TOOLBOX_PATH/simplelink-wifi-toolbox flash-images-builder build tool_settings --key_value programming_instructions=$MATTER_IMG_OUT_DIR/programming_instructions_image.sign.bin actions_req_paths.programming=$MATTER_IMG_OUT_DIR/programming_action_request.sign.bin actions_req_paths.debug=$MATTER_IMG_OUT_DIR/debug_action_request.sign.bin primary_vendor_image=$MATTER_IMG_OUT_DIR/vendor_image.sign.bin primary_ti_wsoc=$MATTER_IMG_OUT_DIR/primary_ti_wireless_fw_image.bin boot_sector=$MATTER_IMG_OUT_DIR/boot_sector_image.bin primary_tbl=$MATTER_IMG_OUT_DIR/primary_tbl_image.bin --output_path $MATTER_IMG_OUT_DIR/tool_settings.json
