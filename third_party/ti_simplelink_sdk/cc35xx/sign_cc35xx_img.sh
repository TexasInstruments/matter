#!/bin/bash

# signs the Matter image so it is flashable
# arg1 is the matter image out directory
# arg2 is the simplelink toolbox path
# arg3 is the generated sysconfig file path
# arg4 is the example name

MATTER_IMG_OUT_DIR=$1
WIFI_TOOLBOX_PATH=$2
SYSCONFIG_GENERATED_FILES_PATH=$3
MATTER_IMG_NAME=$4

$WIFI_TOOLBOX_PATH/simplelink-wifi-toolbox flash-images-builder build programming_image --flash_discovery_config_otfde $SYSCONFIG_GENERATED_FILES_PATH/flash_disc_param_otfde.json --flash_discovery_config_ext_mem $SYSCONFIG_GENERATED_FILES_PATH/external_memory_map.json --flash_discovery_config_xspi $SYSCONFIG_GENERATED_FILES_PATH/flash_disc_param_xspi.json --fuses_programming_instructions $WIFI_TOOLBOX_PATH/programmer/activation_and_initial_programming/jsons/ti_debug/fuse_prog_inst_param.json --dir_out_path $MATTER_IMG_OUT_DIR
$WIFI_TOOLBOX_PATH/simplelink-wifi-toolbox flash-images-builder sign programming_image --unsign_image $MATTER_IMG_OUT_DIR/programming_instructions_image.unsign.bin --private_key $WIFI_TOOLBOX_PATH/programmer/keys/rootCA_privKey.pem --public_key $WIFI_TOOLBOX_PATH/programmer/keys/rootCA_pubKey.pem
$WIFI_TOOLBOX_PATH/simplelink-wifi-toolbox flash-images-builder build action_request --type programming --params_json $SYSCONFIG_GENERATED_FILES_PATH/action_params.json --dir_out_path $MATTER_IMG_OUT_DIR
$WIFI_TOOLBOX_PATH/simplelink-wifi-toolbox flash-images-builder sign action_request --unsign_request $MATTER_IMG_OUT_DIR/programming_action_request.unsign.bin --private_key $WIFI_TOOLBOX_PATH/programmer/keys/rootCA_privKey.pem --public_key $WIFI_TOOLBOX_PATH/programmer/keys/rootCA_pubKey.pem
$WIFI_TOOLBOX_PATH/simplelink-wifi-toolbox flash-images-builder build action_request --type debug --params_json $SYSCONFIG_GENERATED_FILES_PATH/action_params.json --dir_out_path $MATTER_IMG_OUT_DIR
$WIFI_TOOLBOX_PATH/simplelink-wifi-toolbox flash-images-builder sign action_request --unsign_request $MATTER_IMG_OUT_DIR/debug_action_request.unsign.bin --private_key $WIFI_TOOLBOX_PATH/programmer/keys/rootCA_privKey.pem --public_key $WIFI_TOOLBOX_PATH/programmer/keys/rootCA_pubKey.pem
$WIFI_TOOLBOX_PATH/simplelink-wifi-toolbox flash-images-builder build vendor_image --vendor_out_file $MATTER_IMG_OUT_DIR/$MATTER_IMG_NAME --dir_out_path $MATTER_IMG_OUT_DIR
$WIFI_TOOLBOX_PATH/simplelink-wifi-toolbox flash-images-builder sign vendor_image --unsign_image $MATTER_IMG_OUT_DIR/vendor_image.unsign.bin --private_key $WIFI_TOOLBOX_PATH/programmer/keys/rootCA_privKey.pem --public_key $WIFI_TOOLBOX_PATH/programmer/keys/rootCA_pubKey.pem
 
