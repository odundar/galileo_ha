/*
 * Copyright (C) Your copyright notice.
 *
 * Author: Onur Dundar
 *
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "device.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "message.h"
#include "message_queue.h"
#include "thermometer.h"
#include "gasdetector.h"
#include "camera.h"

void delayMinutes(int min) {
	usleep((min * 1000 * 1000 * 60));
	return;
}
void delaySeconds(int sec) {
	usleep((sec * 1000 * 1000));
	return;
}
void emergency_actions() {
	//If There is a Flood Switch Off Wall Plug
	if (strcmp(flood_detector_status, detected) == 0) {
		if (strcmp(wall_plug_switch, device_on) == 0) {
			binary_switch_on_off( WallPlugNodeID, OFF, 0x11);
		}
	}
	//If There is a Gas Leank Switch Off Wall Plug
	if (gas_voltage > GAS_THRESHOLD) {
		if (strcmp(wall_plug_switch, device_on) == 0) {
			binary_switch_on_off( WallPlugNodeID, OFF, 0x11);
		}
	}
	if (strcmp(motion_sensor_status, detected)) {
		//capture_frame();
	}
	if (strcmp(door_sensor_status, door_open)) {
		//capture_frame();
	}
}

void print_device_xml(const char* file_name) {
	FILE* xml_file;
	xml_file = fopen(file_name, "r");
	char line[FILE_LINE];
	while (fgets(line, FILE_LINE, xml_file) != NULL) {
		printf("%s", line);
	}
	fclose(xml_file);
}

void print_device_json(const char* file_name) {
	FILE* json_file;
	json_file = fopen(file_name, "r");
	char line[FILE_LINE];
	while (fgets(line, FILE_LINE, json_file) != NULL) {
		printf("%s", line);
	}
	fclose(json_file);
}
/**
 * Update XML File
 */
void update_device_xml(const char* file_name) {
	FILE* xml_file;
	xml_file = fopen(file_name, "w");
	//TimeStamp for Last Update
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	fprintf(xml_file, "<home>\n");
	fprintf(xml_file, "<last_update_date>%s</last_update_date>\n",
			asctime(timeinfo));
	//Device 1 Temperature Sensor
	fprintf(xml_file,
			"<device id=1>\n\t<name>%s</name>\n\t<status>%s</status>\n\t<temperature>%f</temperature>\n\t<humidity>%f</humidity>\n</device>\n",
			TemperatureSensorName, temperature_sensor_status, temperature_c,
			relative_humidity);
	//Philio Multi Sensor
	fprintf(xml_file,
			"<device id=2>\n\t<name>%s</name>\n\t<status>%s</status>\n\t<temperature>%f</temperature>\n\t<lumination>%f</lumination>\n\t<motion>%s</motion>\n\t<door>%s</door>\n\t<battery>%d</battery>\n\t<power_level>%s</power_level>\n</device>\n",
			MultiSensorNodeName, multi_sensor_status, temperature_f,
			illumination, motion_sensor_status, door_sensor_status,
			multi_sensor_battery_level, multi_sensor_power_level);
	//Wall Plug
	fprintf(xml_file,
			"<device id=3>\n\t<name>%s</name>\n\t<status>%s</status>\n\t<switch>%s</switch>\n\t<power_meter>%f</power_meter>\n\t<energy_meter>%f</energy_meter>\n\t<power_level>%s</power_level>\n</device>\n",
			WallPlugNodeName, wall_plug_status, wall_plug_switch,
			current_energy_consumption, cumulative_energy_level,
			wall_plug_power_level);
	//Lamp Holder
	fprintf(xml_file,
			"<device id=4>\n\t<name>%s</name>\n\t<status>%s</status>\n\t<switch>%s</switch>\n\t<power_level>%s</power_level>\n</device>\n",
			LampHolderNodeName, lamp_holder_status, lamp_holder_switch,
			lamp_holder_power_level);
	//Everspring Flood Detector
	fprintf(xml_file,
			"<device id=5>\n\t<name>%s</name>\n\t<status>%s</status>\n\t<flood>%s</flood>\n\t<battery>%d</battery>\n\t<power_level>%s</power_level>\n</device>\n",
			FloodSensorNodeName, flood_detector_status, flood_sensor_status,
			flood_detector_battery_level, flood_detector_power_level);
	//Gas Detector
	fprintf(xml_file,
			"<device id=6>\n\t<name>%s</name>\n\t<status>%s</status>\n\t<gas_level>%d</gas_level>\n</device>\n",
			GasSensorName, gas_sensor_status, gas_voltage);
	//Network Camera
	fprintf(xml_file,
			"<device id=7>\n\t<name>%s</name>\n\t<status>%s</status>\n\t<port>%s</port>\n</device>\n",
			NetworkCameraName, network_camera_status, network_camera_port);
	fprintf(xml_file, "</home>\n");
	fclose(xml_file);
}
/**
 * Update JSON File
 */
void update_device_json(const char* file_name) {
	FILE* json_file;
	json_file = fopen(file_name, "w");
	//TimeStamp for Last Update
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	char time_buf[BUFFER];
	sprintf(time_buf, "%s", asctime(timeinfo));
	int i = 0;
	while(time_buf[i] != '\n'){
		i++;
	}
	time_buf[i] = ' ';
	fprintf(json_file, "{\n    \"home\": {\n");
	fprintf(json_file, "\t\"last_update_date\": \"%s\",\n\t\"device\": [\n",
			time_buf);
	//Device 1 Temperature Sensor
	fprintf(json_file,
			"\t{\n    \t\"id\": \"1\",\n    \t\"name\": \"%s\",\n    \t\"status\": \"%s\",\n    \t\"temperature\": \"%f\",\n    \t\"humidity\": \"%f\"\n\t},\n",
			TemperatureSensorName, temperature_sensor_status, temperature_c,
			relative_humidity);
	//Philio Multi Sensor
	fprintf(json_file,
			"\t{\n    \t\"id\": \"2\",\n    \t\"name\": \"%s\",\n    \t\"status\": \"%s\",\n    \t\"temperature\": \"%f\",\n    \t\"lumination\": \"%f\",\n    \t\"motion\": \"%s\",\n    \t\"door\": \"%s\",\n    \t\"battery\": \"%d\",\n    \t\"power_level\": \"%s\"\n\t},\n",
			MultiSensorNodeName, multi_sensor_status, temperature_f,
			illumination, motion_sensor_status, door_sensor_status,
			multi_sensor_battery_level, multi_sensor_power_level);
	//Wall Plug
	fprintf(json_file,
			"\t{\n    \t\"id\": \"3\",\n    \t\"name\": \"%s\",\n    \t\"status\": \"%s\",\n    \t\"switch\": \"%s\",\n    \t\"power_meter\": \"%f\",\n    \t\"energy_meter\": \"%f\",\n    \t\"power_level\": \"%s\"\n\t},\n",
			WallPlugNodeName, wall_plug_status, wall_plug_switch,
			current_energy_consumption, cumulative_energy_level,
			wall_plug_power_level);
	//Lamp Holder
	fprintf(json_file,
			"\t{\n    \t\"id\": \"4\",\n    \t\"name\": \"%s\",\n    \t\"status\": \"%s\",\n    \t\"switch\": \"%s\",\n    \t\"power_level\": \"%s\"\n\t},\n",
			LampHolderNodeName, lamp_holder_status, lamp_holder_switch,
			lamp_holder_power_level);
	//Everspring Flood Detector
	fprintf(json_file,
			"\t{\n    \t\"id\": \"5\",\n    \t\"name\": \"%s\",\n    \t\"status\": \"%s\",\n    \t\"flood\": \"%s\",\n    \t\"battery\": \"%d\",\n    \t\"power_level\": \"%s\"\n\t},\n",
			FloodSensorNodeName, flood_detector_status, flood_sensor_status,
			flood_detector_battery_level, flood_detector_power_level);
	//Gas Detector
	fprintf(json_file,
			"\t{\n    \t\"id\": \"6\",\n    \t\"name\": \"%s\",\n    \t\"status\": \"%s\",\n    \t\"gas_level\": \"%d\"\n\t},\n",
			GasSensorName, gas_sensor_status, gas_voltage);
	//Network Camera
	fprintf(json_file,
			"\t{\n    \t\"id\": \"7\",\n    \t\"name\": \"%s\",\n    \t\"status\": \"%s\",\n    \t\"port\": \"%d\"\n\t}\n",
			NetworkCameraName, network_camera_status, network_camera_port);
	fprintf(json_file,"    \t]\n\t}\n}\n");
	fclose(json_file);
}
/**
 * Set Values Defaults for Device and Sensor Values
 * @param void
 * @return void
 */
void set_defaults(void) {
	snprintf(multi_sensor_status, sizeof(status_sleeping), status_sleeping);
	snprintf(wall_plug_status, sizeof(status_sleeping), status_sleeping);
	snprintf(lamp_holder_status, sizeof(status_sleeping), status_sleeping);
	snprintf(flood_detector_status, sizeof(status_sleeping), status_sleeping);
	snprintf(temperature_sensor_status, sizeof(status_active), status_active);
	snprintf(gas_sensor_status, sizeof(status_active), status_active);
	sprintf(multi_sensor_power_level, "Normal");
	sprintf(wall_plug_power_level, "Normal");
	sprintf(lamp_holder_power_level, "Normal");
	sprintf(flood_detector_power_level, "Normal");
	snprintf(lamp_holder_switch, sizeof(device_off), device_off);
	snprintf(wall_plug_switch, sizeof(device_off), device_off);
	snprintf(flood_sensor_status, sizeof(not_detected), not_detected);
	snprintf(door_sensor_status, sizeof(door_closed), door_closed);
	snprintf(motion_sensor_status, sizeof(not_detected), not_detected);
	snprintf(network_camera_status, sizeof(status_sleeping), status_active);
	sprintf(network_camera_port, "3200");
	multi_sensor_battery_level = 0;
	flood_detector_battery_level = 0;
	temperature_f = 0.0;
	temperature_c = 0.0;
	relative_humidity = 0.0;
	illumination = 0;
	co_level = 0;
	current_energy_consumption = 0.0;
	cumulative_energy_level = 0.0;
}
/**
 * Periodic update worker
 * @param null
 * @return null
 */
void* update_status(void* arg) {
	set_defaults();
	while (1) {
		/**
		 * Periodic Updates from Connected Devices
		 */
		/**
		 * Philio Multi-Sensor Node Commands
		 */
		get_device_status( MultiSensorNodeID, 1);
		get_battery_level( MultiSensorNodeID, 2);
		get_binary_sensor_value( MultiSensorNodeID, DOOR_WINDOW_SENSOR, 3);
		get_binary_sensor_value( MultiSensorNodeID, MOTION_DETECTION_SENSOR, 4);
		get_multilevel_sensor_value( MultiSensorNodeID, TEMPERATURE_SENSOR, 5);
		get_multilevel_sensor_value( MultiSensorNodeID, LUMINANCE_SENSOR, 6);
		get_node_power_level( MultiSensorNodeID, 7);
		/**
		 * Fibaro Wall Plug Commands
		 */
		get_device_status( WallPlugNodeID, 8);
		get_binary_switch_status( WallPlugNodeID, 9);
		get_meter_level( WallPlugNodeID, POWER, 10);
		get_meter_level( WallPlugNodeID, ENERGY, 11);
		get_node_power_level(WallPlugNodeID, 12);
		/**
		 * Everspring Lamp Holder Commands
		 */
		get_device_status( LampHolderNodeID, 13);
		get_binary_switch_status( LampHolderNodeID, 14);
		get_node_power_level(LampHolderNodeID, 15);
		/**
		 * Everspring Flood Detector Commands
		 */
		get_device_status( FloodSensorNodeID, 16);
		get_sensor_alarm_value( FloodSensorNodeID, FLOOD_ALARM, 17);
		get_node_power_level(FloodSensorNodeID, 18);
		/**
		 * Read From Sensors Connected to Intel Galileo
		 * SHT11 and MQ-9
		 */
		delaySeconds(2);
		snprintf(temperature_sensor_status, sizeof(status_active),
		status_active);
		//temperature_c = read_temperature();
		temperature_c = 24.0;
		//delaySeconds(2);
		//relative_humidity = read_humidity();
		relative_humidity = 70.0;
		//delaySeconds(2);
		snprintf(gas_sensor_status, sizeof(status_active), status_active);
		//co_level = read_gas_sensor();
		co_level = 2000;
		/**
		 * Check for Emergency Actions
		 */
		emergency_actions();
		/**
		 * Sleep for 2 Minutes Print Report to devices.xml file
		 */
		update_device_xml(XML_FILE_NAME);
		update_device_json(JSON_FILE_NAME);
		delayMinutes(UPDATE_FREQUENCY);
	}
	return NULL;
}

