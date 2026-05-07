source ./scripts/activate.sh
cd examples/lighting-app/ti/cc35xx
gn gen out/debug --args="ti_sysconfig_root=\"$HOME/ti/sysconfig_1.26.3\" ti_simplelink_wifi_toolbox_root=\"$HOME/ti/simplelink_wifi_toolbox_lin_4_1_16\" ti_simplelink_wifi_sdk_root=\"$HOME/ti/simplelink_wifi_sdk_10_10_00_18\""
ninja -C out/debug
cd -
