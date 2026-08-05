'''
A Python wrapper to sign the CC35xx Matter image

The example's out/ directory, the Simplelink Wi-Fi Toolbox path and the
example's sysconfig generated files path are passed in the shell script
is executed
'''

import subprocess
import sys

# pass in path to the shell file
script_path = "./" + sys.argv[1] + "sign_cc35xx_img.sh"

print(script_path)

# argv[2..6] are the 5 positional path args forwarded to the shell script.
# argv[7] is matter_software_ver as a hex uint32 string (e.g. "0x01000001").
# Decode it to MAJOR.MINOR.PATCH.BUILD for the toolbox --version flag (BUILD is hardcoded to 0).
ver = int(sys.argv[7], 16)
major = (ver >> 24) & 0xFF
minor = (ver >> 16) & 0xFF
patch =  ver        & 0xFFFF
build = int(0)
toolbox_ver = f"{major}.{minor}.{patch}.{build}"

script_args = sys.argv[2:7] + [toolbox_ver]

# exec
ret = subprocess.run([script_path] + script_args)
