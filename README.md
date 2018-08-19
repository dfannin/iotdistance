# iotdistance
IOT Distance Sensor for ESP8266 (ESP201) Wireless 

## Introduction

This project is an Internet-of-Things (IoT) distance sensor that can be used to measure
the proximity and distance of objects. Using the System on a Chip (SoC) ESP3266 
process, which has a native 802.11 interface (WiFi), it provides distance information via built-in http server, and JSON formatted responses.  

One application is for garage door status sensing, another use is as a parking 
distance sensor. It can be built on a single board, and packaged in a small enclosure.
A Nagios check script was written, so that distance changes can be logged
and alerted via a Nagios monitoring server. 

The devices mates the an Ultrasonic distance sensor (cheap and plentiful HC-SRC04) and
ESP8266 module (ESP201) for an Internet-of-Things (IoT) distance sensor that is easy 
to build and use.  It uses the Arduino IDE environment, along with the Arduino ESP8266 board manager extension, for development. 

The ESP201 module requires an external resistor, button, and switch for performing firmware loading, and reset.  The circuit schematic provides a re-usable design for 
the ESP201 module, as well as dual power supply, and 5v to 3.3v level shifting for 
the Ultrasonic module. 

The device software also includes Over-The-Air (OTA) firmware updates.  Install the `php` script (tools/esp8266 directory) to an Apache server, and copy the desired .ino.bin file, and update the script to include the
device MAC address and firmware name.  You can then use the '/distance/updatefirmware' api query to
load the new firmware and reboot automatically.  


## Features

+ distance sensor (1cm to 4 meters)
+ http query/response with JSON responses (ReST format)
+ WiFi interface and automatic association
+ helper scripts for command line queries and Nagios integration
+ OTA updates feature - remotely update firmware (http server using php script) 

## Protocol

Query: `http://<ip address>[:80]/distance/info`

Response:
```json
{ "info": 1,
   "ipaddr": "192.168.20.10", 
   "ssid": "mygateway", 
   "rssi": -81, 
   "mac": "5c:6a:e:7f:cf:55", 
   "version": "1.1g", 
   "sensor": "distance"
}
```

Query: http://<ip address>[:80]/distance/get

Response:
```json
{  "get": 1,
   "distance": 304,
   "mac": "5c:6a:e:7f:cf:5c",
   "version": "1.1g", 
   "sensor": "distance"
}
```

Query: http://<ip address>[:80]/distance/updatefirmware

Response:
```json
{  "update": 1,
   "ipaddr": "192.168.20.10", 
   "mac": "5c:6a:e:7f:cf:5c",
   "version": "1.0g", 
   "sensor": "distance"
}
```

Invalid Response:
```json
{  "invalid": 1,
   "request": "GET /distance/invalid", 
   "mac": "5c:6a:e:7f:cf:5c",
   "version": "1.0g", 
   "sensor": "distance"
}
```

## Installation

The .ino sketch should be loaded and compiled for the ESP8266.

The script needs to be updated to provide the SSID and password 
for your WiFi network. 

The firmware loading procedure:

+ Connect a FTDI usb adapter to the 6 pin header on the board.
+ Set the switch to *program* mode
+ press the reset button
+ click the *upload* button on the Arduino IDE
+ the firmware should upload to the device (blinkenlights) 
+ when its done, set the switch to *run* mode
+ press the reset button to start the new firmware.

The IoT device should then associate with the network, and
get an ip address.  The device also responds to ping messages (ICMP).


## References

+ [ESP201 First Impressions](https://goo.gl/RtJsE4)  review of the module
+ [Programming ESP201 with Arduino](https://goo.gl/RtJsE4) how to set up programming 
+ [ESP201 Cheat Sheet](https://goo.gl/9vsDGU) one page cheat sheet
+ [Setting up ESP8266 Programmin on Arduino](https://goo.gl/exf5hq) SparkFun's overview
+ [Nagios Monitoring](https://www.nagios.org/)
