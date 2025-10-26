# Web Interface

The data logger now has a simple web interface located in the SPIFFS partition of the ESP32. The web page uses [Axios](https://axios-http.com/) and [Vue.js](https://vuejs.org/) to access the [REST API](https://restfulapi.net/) of the data logger. Data is exchanged using [JSON](https://arduinojson.org/) arrays and objects except for the log files themselves (they are plain/text). To connect to the data logger please start Wifi by pushing the button on the front panel (the yellow led wil light up). You can then search for a Wifi network called **Datalogger_XXXXXX** on your computer (XXXXXX being part of the hardware id of your ESP32). The default Wifi password is **12345678**. After connecting you can direct your browser to **http://datalogger.local** (or http://10.0.0.1).

![Web Interface](images/Webinterface.png)

## 1
The login button in the header does a simple HTTP basic authentication (a GET on /api/login) to authenticate the admin user. You need to login to e.g. access the **Save** buttons.

## 2
In section two you can syncronize the data loggers RTC (Real Time Clock) with the date and time of the computer where your browser is running. Syncronization needs to be done once in a while to prevent the data loggers RTC from drifting too much.

## 3
Here you can set the timer that queries modbus (the sheduler of the data logger). If you set the value to e.g. one minute the data logger will read the modbus registers configured in section six every minute. A value of zero minutes will disable the timer. Before starting the timer it is probably best to configure and test all modbus configurations in section six. Otherwise the log files will contain a lot of errors.

## 4
In section four you can take a look at the log files written to the SD card by selecting the year, month and day. You can also **Refresh**, **Download** and **Delete** a log file.

## 5
In this section you can change the Wifi user and password and also the HTTP user and password (the **Login** button from section one). Please remember changing the HTTP user/password has an immediate effect (you have to reauthenticate). Changing the Wifi SSID and password will not take effect until you stop/start Wifi (or power cycle the data logger).

## 6
Here you can configure all the modbus registers the data logger periodically queries. A lot of features here don't work yet as of now. For example you can only use **Function Code 4**. And most of the Finder value types have not been implemented (only T1, T2, T3 and T_float). You can use the **Test** button to verify the configurations before you start the sheduler in section three. A device address of zero or a value type of FOO will mark the configuration unused.

## 7
And last but not least you can set the UART parameters used for modbus comunication. These values depend on the bus you are connecting to. Default Finder energy meter values are 19200 baud and eight data bits, no parity and two stop bits (8N2). Please remember you can terminate modbus with 120 Ohms by opening or closing the jumper on the right side of the PCB (Printed Circuit Board). The data logger is a modbus client (also called master). Changing the UART parameters will only take effect after stop/starting Wifi (rebooting or waking the data logger from deep sleep).
