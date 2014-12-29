galileo_ha
==========

Home Automation Software For Intel Galileo

This Folder Includes the Source Code for Home Automation with Intel Galileo book complete sample codes published in the book. 

galileo_ha is the acronym for Intel Galileo Home Automation

Subdirectories, Applications or Projects:

        \thermometer
An application to read temperature and relative humidity from Sensirion SHT11 sensor connected to Intel Galileo GPIO Pins. Chapter 3.

        \serialcomm
An application to read and write data to/from a usb device connected from usb port and uses serial communication with Intel Galileo. Chapter 3.

        \energymanager
An application to control devices related with energy management, zwave wall plug, energy meter, lamp holder, illumination sensor, temperature sensor with a basic
terminal user interface. Chapter 4.

        \gasdetector
A Simple application to work with gas detection sensor with Intel Galileo. Chapter 5.

        \securehome
Built upon energymanager application with security additions, camera, motion, door window sensor, gas detector, flood sensor additions. Chapter 5.

        \ipcamera
An application to connect with a network camera an use it with OpenCV. Chapter 6.

        \smarthome
Built upon merging energy manager and secure home application together with camera application. Added feature to define rules for security cases. Chapter 6, 7, 8...

        \smarthome_app
Android app to connect Intel Galileo smarthome app to send command and receive notifications. Chapter 8.

        \webinterface
A basic web interface to manage smarthome application on Galileo, run by NodeJS. Chapter 8.
