#!/bin/sh
# pip install paho-mqtt

# Enable OTA in config.json
# Do not forget to increment firmware version

if pio run ; then
    echo "Build sucessful. Running ota update."
    python ota_updater.py -l 192.168.5.100 -t "" -i "homie/flipdot" .pio/build/esp32doit-devkit-v1/firmware.bin
else
    echo "Build unsucessful. Not running ota update."
fi
