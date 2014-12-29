/*
 * device.c
 *
 *  Created on: Dec 17, 2014
 *      Author: onur
 */
#include "device.h"
#include <stdlib.h>
#include <stdio.h>
#include "message.h"

void delayMinutes(int min) {
	usleep((min * 1000 * 1000 * 60));
	return;
}
void delaySeconds(int sec) {
	usleep((sec * 1000 * 1000));
	return;
}
void emergency_actions(int device_file) {
	//If There is a Flood Switch Off Wall Plug
	if (flood_sensor_status == ON) {
		if (wall_plug_switch == ON) {
			binary_switch_on_off(device_file, WallPlugNodeID, OFF, 0x11);
		}
	}
}
void print_sensor_report() {
	printf("HOME STATUS\n");
	printf("Multi-Sensor Status is ");
	if (multi_sensor_status == ON) {
		printf("Active\n");
	} else {
		printf("Sleeping\n");
	}
	printf("Door/Window Sensor is ");
	if (door_sensor_status == ON) {
		printf("OPEN\n");
	} else {
		printf("CLOSED\n");
	}
	printf("Motion ");
	if (door_sensor_status == ON) {
		printf("Detected\n");
	} else {
		printf("Not Detected\n");
	}
	printf("Temperature is %f Fahrenheit\n", temperature);
	printf("Illumination is %f Lux\n", illumination);
	printf("Wall Plug Status is ");
	if (wall_plug_status == ON) {
		printf("Active\n");
	} else {
		printf("Sleeping\n");
	}
	printf("Wallplug Switch is ");
	if (wall_plug_switch == ON) {
		printf("ON\n");
	} else {
		printf("OFF\n");
	}
	printf("Current Energy Consumption is %f Watts\n",
			current_energy_consumption);
	printf("Cumulative Energy Consumption is %f kWatts hours \n",
			cumulative_energy_level);
	printf("Lamp Holder Status is ");
	if (wall_plug_status == ON) {
		printf("Active\n");
	} else {
		printf("Sleeping\n");
	}
	printf("Lampholder Switch is ");
	if (lamp_holder_switch == ON) {
		printf("ON\n");
	} else {
		printf("OFF\n");
	}
	printf("Flood Detector Status is ");
	if (flood_detector_status == ON) {
		printf("Active\n");
	} else {
		printf("Sleeping\n");
	}
	printf("Flood ");
	if (flood_sensor_status == ON) {
		printf("DETECTED\n");
	} else {
		printf("Not Detected\n");
	}
}
void* update_status(int device_file) {

	while (1) {
		/**
		 * Request Updates
		 */
		//Multi-Sensor
		get_device_status(device_file, MultiSensorNodeID, 1);
		delaySeconds(10);
		get_battery_level(device_file, MultiSensorNodeID, 2);
		delaySeconds(10);
		get_binary_sensor_value(device_file, MultiSensorNodeID,
				DOOR_WINDOW_SENSOR, 3);
		delaySeconds(10);
		get_binary_sensor_value(device_file, MultiSensorNodeID,
				DOOR_WINDOW_SENSOR, 4);
		delaySeconds(10);
		get_multilevel_sensor_value(device_file, MultiSensorNodeID,
				TEMPERATURE_SENSOR, 5);
		delaySeconds(10);
		get_multilevel_sensor_value(device_file, MultiSensorNodeID,
				LUMINANCE_SENSOR, 6);
		delayMinutes(1);
		//Wall Plug
		get_device_status(device_file, WallPlugNodeID, 7);
		delaySeconds(10);
		get_binary_switch_status(device_file, WallPlugNodeID, 8);
		delaySeconds(10);
		get_meter_level(device_file, WallPlugNodeID, POWER, 9);
		delaySeconds(10);
		get_meter_level(device_file, WallPlugNodeID, ENERGY, 10);
		delayMinutes(1);
		//Lamp Holder
		get_device_status(device_file, LampHolderNodeID, 11);
		delaySeconds(10);
		get_binary_switch_status(device_file, LampHolderNodeID, 12);
		delaySeconds(30);
		//Flood Detector
		get_device_status(device_file, FloodSensorNodeID, 13);
		delaySeconds(10);
		get_sensor_alarm_value(device_file, FloodSensorNodeID, FLOOD_ALARM, 14);
		delaySeconds(10);
		/**
		 * Check for Emergency Actions
		 */
		emergency_actions(device_file);
		print_sensor_report();
		/**
		 * Sleep for 5 Minutes
		 */
		delayMinutes(5);
	}
	return NULL;
}

