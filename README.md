# ESP8266_IR_MQTT
Simple IR to MQTT bridge, used to trigger OpenHAB events from a Logitech Harmony remote.

Based on a Heltec ESP8266 with OLED display: ( https://heltec.org/project/wifi-kit-8/ ) and a VS1838B IR receiver connected to pin D3.

Forwards all IR commands that match a specified address (https://github.com/richardbounds/ESP8266_IR_MQTT/blob/main/ESP8266_IR_MQTT.ino#L35 ) to an MQTT topic (https://github.com/richardbounds/ESP8266_IR_MQTT/blob/main/ESP8266_IR_MQTT.ino#L21). I have used address 17795, which matches a dummy IR remote for a Sherwood R-125RDS (random pick), configured in the Harmony hub. I found it convenient to use the Power on/off commands (38/34) to trigger scenes when Harmony activities including the dummy device start/stop.

IR signals received are shown on the OLED display and logged to the serial monitor.
