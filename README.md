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

        \ip_cam_capture
An application to connect with a network camera and use it with OpenCV to capture a frame. Chapter 6.

	\ip_cam_recorder
An application to connect with a network camera and use it with OpenCV to record video. Chapter 6.

	\usb_cam_capture
An application to connect with an USB camera and use it with OpenCV to capture a frame. Chapter 6.

        \smarthome
Built upon merging energy manager and secure home application together with camera application. Added feature to define rules for security cases. Chapter 6, 7, 8...

	\smarthome-0.0.1
Includes the autotools auto generated files with source code

        \smarthome_app
Android app to connect Intel Galileo smarthome app to send command and receive notifications. Chapter 8.

        \webinterface
A basic web interface to manage smarthome application on Galileo, run by NodeJS. Chapter 8.

	smarthome-0.0.1.tar.gz
smarthome-0.0.1 folder compressed into this file to use with yocto recipe.

	\smarthome-0.0.1-yocto-recipe
Includes the Yocto Project meta data file aka recipe to build smarthome application with Yocto Project and run as a service.
