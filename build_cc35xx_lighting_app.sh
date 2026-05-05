source ./scripts/activate.sh
cd examples/lighting-app/ti/cc35xx
gn gen out/debug --args="ti_sysconfig_root=\"$HOME/ti/sysconfig_1.27.0_4552\" ti_simplelink_wifi_toolbox_root=\"$HOME/ti/simplelink_wifi_toolbox_lin_4_1_8\""
cd -
