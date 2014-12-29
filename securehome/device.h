/*
 * device.h
 *
 *  Created on: Dec 17, 2014
 *      Author: onur
 */

#ifndef DEVICE_H_
#define DEVICE_H_


/**
 * Transmit Option for ZWave Controller
 */
#define TRANSMIT_OPTION									0x25

/**
 * Defined Nodes
 */
#define NumberofNodes 			5
#define ControllerNodeID 		0x01
#define MultiSensorNodeID 		0x02
#define WallPlugNodeID 			0x03
#define LampHolderNodeID 		0x04
#define FloodSensorNodeID		0x05

/**
 * Device Status
 */
static int multi_sensor_status = 0;
static int wall_plug_status = 0;
static int lamp_holder_status = 0;
static int flood_detector_status = 0;
/**
 * Battery Levels
 */
static int multi_sensor_battery_level = 0;
static int flood_sensor_battery_level = 0;
/**
 * Current Environment Status
 */
static float temperature = 0.0;
static float illumination = 0.0;
static float relative_humidity = 0.0;
static float co_level = 0.0;
static float co2_level = 0.0;
static float methane_level = 0.0;

/**
 * Energy Status
 */
static float current_energy_consumption = 0.0;
static float cumulative_energy_level = 0.0;

/**
 * Switch Status
 */
static int lamp_holder_switch = 0;
static int wall_plug_switch = 0;

/**
 * Security Sensor Status
 */
static int flood_sensor_status = 0;
static int door_sensor_status = 0;
static int motion_sensor_status = 0;

void* update_status(int device_file);

#endif /* DEVICE_H_ */
