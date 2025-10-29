# REST API

After experimenting with a ftp/telnet server for a while it soon dawned on me that the ESP32 wants to programmed using a web server. I never liked the way most programs mix code and html. After some searching I rediscovered an oldie but goodie: The [REST](https://en.wikipedia.org/wiki/REST) API (Application Programming Interface). It has some nice features like being client/server, stateless and more. It's also loosely based on the [CRUD](https://en.wikipedia.org/wiki/Create,_read,_update_and_delete) concept. The most important features for me include separating html and binary code. REST URLs can also easily be scripted. And if you have them work with JSON objects a web interface can be added on top. All examples below use the nifty [cURL](https://curl.se/) command line utility. 

## Overview

![RESTful](images/RESTful.png)

## RTC
The following GET queries the RTC (Real Time Clock) and returns the date in UTC (Universal Time Coordinates) epoch seconds.
<pre>
curl http://datalogger.local/api/rtc
1752140389
</pre>

The following PUT sets the date:
<pre>
curl -u "admin:admin" -X PUT -d "epoch=1747931902" http://datalogger.local/api/rtc
</pre>

## Timer
The following GET returns the timer value in minutes. The timer of the PCF8563 RTC is the scheduler waking the ESP32 at regular intervals to read values via Modbus. It is an 8 bit timer clocked at 1/60 Hz which results in an interrupt every 1 to 255 minutes (4 hours and 15 minutes max).
<pre>
curl http://datalogger.local/api/timer
1
</pre>

And the next PUT sets the timer value. A value of 0 disables the timer. The example below changes the timer interval to 5 minutes:
<pre>
curl -u "admin:admin" -X PUT -d "minutes=5" http://datalogger.local/api/timer
</pre>

## Logs
To get access to the log files you have to first query the available years:
<pre>
curl http://datalogger.local/api/logs
2025
</pre>

From there you can check the available months in that year:
<pre>
curl http://datalogger.local/api/logs/2025
04
05
06
</pre>

And lastly you can list the daily log files with their file size:
<pre>
http://datalogger.local/api/logs/2025/04
01 62351
02 63082
03 63171
04 62853
.
.
.
</pre> 

Downloading a log file is a simple GET on the file:
<pre>
curl http://datalogger.local/api/logs/2025/04/21
20250421000000 54505 1221119 237 555788 0.2
20250421000100 54505 1221119 237 555791 0.2
20250421000200 54505 1221119 237 555793 0.3
.
.
.
</pre>

Uploading to an existing log file is a POST:
<pre>
curl -u "admin:admin" -F "file=@21" http://datalogger.local/api/logs/2025/04/21
</pre>  

And deleting a log file is a DELETE with authentication:
<pre>
curl -u "admin:admin" -X DELETE http://datalogger.local/api/logs/2025/04/21
</pre>

## System

To get and change the Wifi SSID, Wifi password, http user and http password you can use the following statements. A GET returns all the configurable parameters:
<pre>
curl -u "admin:admin" http://datalogger.local/api/system
wifiSSID=Datalogger_5AB278&wifiPassword=&httpUser=admin&httpPassword=admin
</pre>

A PUT on one or multiple parameters changes the default settings. Remember changing the http password changes access to the REST API immediately. Changing the Wifi password requires rebooting the data logger (or waking it from deep sleep). Please also note a Wifi password needs to be at least eight characters in length. Otherwise the ESP will behave like it has no password set.
<pre>
curl -u "admin:admin" -X PUT -d "wifiPassword=12345678" http://datalogger.local/api/system
</pre>

To reset the system the following DELETE will load default values for all system, serial and Modbus settings:
<pre>
curl -u "admin:admin" -X DELETE http://datalogger.local/api/system
</pre>

You should also reset the system whenever you change the number of Modbus configurations stored in NVS (Non Volatile Storage).

## Modbus

The following GET will list the number of available Modbus configurations. The number can be changed in the Settings.h header file. Please remember not to configure too many slots otherwise they will not fit into the NVS partition of the ESP32.
<pre>
curl http://datalogger.local/api/modbus
0
1
2
3
4
</pre>

A default configuration looks something like this. A device address of 0 and a value type named `NYI` (not yet implemented) means the configuration is not used.
<pre>
curl http://datalogger.local/api/modbus/0/config
deviceAddress=0&functionCode=0&amp;registerAddress=0&valueType=NYI
</pre>

The following PUT will set a configuration:
<pre>
curl -u "admin:admin" -X PUT -d "deviceAddress=33&functionCode=4&amp;registerAddress=462&valueType=T3" http://datalogger.local/api/modbus/0/config
</pre>

And with the following GET you can test if the configuration works. Remember to enable the scheduler (timer) once the configurations work the way you want for the values to be logged to the SD card.
<pre>
curl http://datalogger.local/api/modbus/0
55947
</pre>

## Firmware
To OTA (Over The Air) upgrade the firmware of the data logger you first need to upload a new firmware.bin file to the SD card (POST). The second curl (PUT) starts the actual upgrade. A command value of 0 equals the `U_FLASH` define in Arduinos Update.h header file (you need to let Arduino Update know for which partition this binary is). Once you start the upgade the yellow led will start blinking rapidly and at the end of a successfull upgrade the ESP32 will reboot.
<pre>
curl -u "admin:admin" -F "file=@firmware.bin" http://datalogger.local/api/firmware
curl -u "admin:admin" -X PUT -d 'command=0' http://datalogger.local/api/firmware
</pre>

The procedure for upgrading the web pages in SPIFFS is similar. Generate the spiffs.bin file with a `pio run -t buildfs`. Upload the binary to the SD card. And then start the upgrade with the PUT. A command value of 100 equals the U_SPIFFS define for the SPIFFS partition.
<pre>
curl -u "admin:admin" -F "file=@spiffs.bin" http://datalogger.local/api/firmware
curl -u "admin:admin" -X PUT -d 'command=100' http://datalogger.local/api/firmware
</pre>

To check the version (GIT hash) currently running on the data logger you can issue the following GET:
<pre>
curl http://datalogger.local/api/firmware
</pre>

## Serial1

Using the following GET you can view the serial port configuration driving Modbus:
<pre>
curl http://datalogger.local/api/serial1
baud=19200&config=8N2
</pre>

And the following PUT changes the configuration of Serial Port 1. Remember to power cycle or wake the data logger from deep sleep for the new values to take effect.
<pre>
curl -u "admin:admin" -X PUT -d "baud=9600&config=8N1" http://datalogger.local/api/serial1
</pre>

## Login

To do a HTTP basic authentication you can do a GET on the /api/login URL. The GET does not return any data. It just returns the status 200 if the authentication was successful and otherwise a 401.

<pre>
curl -D - -u "admin:wrong" http://datalogger.local/api/login 
HTTP/1.1 401 Unauthorized
</pre>