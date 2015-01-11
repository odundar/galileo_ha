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
package com.galileha.smarthome;

import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.HttpURLConnection;
import java.net.InetAddress;
import java.net.MalformedURLException;
import java.net.Socket;
import java.net.URL;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.Scanner;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

/**
 * Activity Class to Populate Views for Android Application
 * Create threads for network operations
 */
public class SmartHome extends Activity {

	private URL homeJSONUrl;
	private URL commandsJSONUrl;
	private String lastUpdateDate;
	private JSONObject homeInfo;
	private String[] commands;
	private String[] deviceNames;
	private String[] deviceStatus;
	private Integer[] deviceImages = { R.drawable.sht11, R.drawable.philio,
			R.drawable.fibaro, R.drawable.everspring, R.drawable.flood,
			R.drawable.mq9, R.drawable.dlink };

	private Socket galileSocket;
	private static final int GALILEOPORT = 3500;
	private static final String GALILEO_IP = "192.168.2.235";

	volatile public boolean filesdownloaded = false;
	volatile public boolean socketcreated = false;

	ArrayList<DeviceItem> listItems;

	ListView deviceListView;
	TextView updateDateText;
	
	/**
	 * Create Socket Connection Thread
	 */
	Thread socketThread = new Thread(new Runnable() {
		@Override
		public void run() {
			// TODO Auto-generated method stub
			InetAddress serverAddr;
			try {
				serverAddr = InetAddress.getByName(GALILEO_IP);
				galileSocket = new Socket(serverAddr, GALILEOPORT);
				socketcreated = true;
			} catch (UnknownHostException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	});
	/**
	 * JSON Worker to Fetch JSON Files from Intel Galileo
	 */
	Thread jsonFetcher = new Thread(new Runnable() {
		@Override
		public void run() {
			// TODO Auto-generated method stub
			try {
				// Initialize URLs
				homeJSONUrl = new URL("http://192.168.2.235:3000/home.json");
				commandsJSONUrl = new URL(
						"http://192.168.2.235:3000/commands.json");
				getHomeStatus();
				getCommandsList();
				// Fill String Arrays
				initalizeHomeArray();
				fillListAdapter();
				filesdownloaded = true;
				Log.d("JSON", "JSON Files Fetched....");
			} catch (MalformedURLException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (JSONException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	});
	
	/**
	 * Update Adapter
	 * Periodically Read the home.json File and Update ListView Adapter
	 */
	Thread updater = new Thread(new Runnable() {

		@Override
		public void run() {
			// TODO Auto-generated method stub
			while (true) {
				try {
					try {
						Thread.sleep(10000);
					} catch (InterruptedException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
					// Initialize URLs
					homeJSONUrl = new URL("http://192.168.2.235:3000/home.json");
					getHomeStatus();
					// Fill String Arrays
					updateHomeArray();
					// Clear Adapter
					listItems.clear();
					fillListAdapter();
					Log.d("JSON", "Updated ListView Adapter....");
				} catch (MalformedURLException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (JSONException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}
	});

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_smart_home);
		deviceListView = (ListView) findViewById(R.id.list);
		updateDateText = (TextView) findViewById(R.id.updateDate);
		listItems = new ArrayList<DeviceItem>();
		jsonFetcher.start();
		while (!filesdownloaded)
			;
		HomeDeviceAdapter deviceAdapter = new HomeDeviceAdapter(this, listItems);
		deviceListView.setAdapter(deviceAdapter);
		deviceListView.setOnItemClickListener(new OnItemClickListener() {
			@Override
			public void onItemClick(AdapterView<?> parent, View view,
					int position, long id) {
				JSONObject homeDevices;
				JSONArray devices;
				JSONObject clickedObject = null;
				try {
					homeDevices = homeInfo.getJSONObject("home");
					devices = (JSONArray) homeDevices.getJSONArray("device");
					clickedObject = devices.getJSONObject(position);
				} catch (JSONException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				// Switch ON/OFF Wall Plug or LampHolder
				if (position == 2) {
					try {
						if (clickedObject.getString("switch").compareTo("ON") == 0) {
							writeToSocket(commands[5]);
						} else {
							writeToSocket(commands[4]);
						}
					} catch (JSONException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				} else if (position == 3) {
					try {
						if (clickedObject.getString("switch").compareTo("ON") == 0) {
							writeToSocket(commands[7]);
						} else {
							writeToSocket(commands[6]);
						}
					} catch (JSONException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				} else {
					Toast.makeText(SmartHome.this,
							"No Available Command for Selected Device",
							Toast.LENGTH_LONG).show();
				}
			}
		});
		updateDateText.setText(lastUpdateDate);
		socketThread.start();
		while (!socketcreated)
			;
		updater.start();
		Log.d("SOCKET", "Socket Thread Started");
	}

	/**
	 * Read home.json from Server
	 * 
	 * @param URL
	 * @return
	 * @throws IOException
	 *             , MalformedURLException, JSONException
	 */
	public void getHomeStatus() throws IOException, MalformedURLException,
			JSONException {
		// Set URL
		// Connect to Intel Galileo get Device Status
		HttpURLConnection httpCon = (HttpURLConnection) homeJSONUrl
				.openConnection();
		httpCon.setReadTimeout(10000);
		httpCon.setConnectTimeout(15000);
		httpCon.setRequestMethod("GET");
		httpCon.setDoInput(true);
		httpCon.connect();
		// Read JSON File as InputStream
		InputStream readStream = httpCon.getInputStream();
		Scanner scan = new Scanner(readStream).useDelimiter("\\A");
		// Set stream to String
		String jsonFile = scan.hasNext() ? scan.next() : "";
		// Initialize serveFile as read string
		homeInfo = new JSONObject(jsonFile);
		httpCon.disconnect();
	}

	/**
	 * Initialize String Array from JSON
	 * 
	 * @throws JSONException
	 */
	public void initalizeHomeArray() throws JSONException {
		JSONObject homeDevices = homeInfo.getJSONObject("home");
		lastUpdateDate = homeDevices.getString("last_update_date");
		JSONArray devArray = (JSONArray) homeDevices.getJSONArray("device");
		int numOfDevices = devArray.length();
		deviceNames = new String[numOfDevices];
		deviceStatus = new String[numOfDevices];
		// Fill the Name Array
		for (int i = 0; i < numOfDevices; i++) {
			JSONObject devObject = (JSONObject) devArray.get(i);
			deviceNames[i] = devObject.getString("name");
		}
		// Fill the Status Array
		updateHomeArray();
	}

	/**
	 * Update Home Status Array with Reading from JSON Object
	 * 
	 * @throws JSONException
	 */
	public void updateHomeArray() throws JSONException {
		JSONObject homeDevices = homeInfo.getJSONObject("home");
		lastUpdateDate = homeDevices.getString("last_update_date");
		JSONArray devices = (JSONArray) homeDevices.getJSONArray("device");
		// SHT11
		JSONObject devObject = (JSONObject) devices.get(0);
		deviceStatus[0] = new String("Status: " + devObject.getString("status")
				+ "\nTemp: " + devObject.getString("temperature")
				+ "\nHumidity: " + devObject.getString("humidity"));
		Log.d("HOME", deviceStatus[0]);
		// Philio
		devObject = (JSONObject) devices.get(1);
		deviceStatus[1] = new String("Status: " + devObject.getString("status")
				+ "\nTemp: " + devObject.getString("temperature")
				+ "\nLumination: " + devObject.getString("lumination")
				+ "\nMotion: " + devObject.getString("motion")
				+ "\nDoor/Window: " + devObject.getString("door")
				+ "\nBattery: " + devObject.getString("battery")
				+ "\nPowerLevel: " + devObject.getString("power_level"));
		Log.d("HOME", deviceStatus[1]);
		// Wall Plug
		devObject = (JSONObject) devices.get(2);
		deviceStatus[2] = new String("Status: " + devObject.getString("status")
				+ "\nSwitch: " + devObject.getString("switch")
				+ "\nCurrent Consumption: "
				+ devObject.getString("power_meter")
				+ "\nCumulative Consumption: "
				+ devObject.getString("energy_meter") + "\nPowerLevel: "
				+ devObject.getString("power_level"));
		// Lamp Holder
		devObject = (JSONObject) devices.get(3);
		deviceStatus[3] = new String("Status: " + devObject.getString("status")
				+ "\nSwitch: " + devObject.getString("switch")
				+ "\nPowerLevel: " + devObject.getString("power_level"));
		// Flood Detector
		devObject = (JSONObject) devices.get(4);
		deviceStatus[4] = new String("Status: " + devObject.getString("status")
				+ "\nFlood: " + devObject.getString("flood") + "\nBattery: "
				+ devObject.getString("battery") + "\nPowerLevel: "
				+ devObject.getString("power_level"));
		// GAS Detector
		devObject = (JSONObject) devices.get(5);
		deviceStatus[5] = new String("Status: " + devObject.getString("status")
				+ "\nGas Level: " + devObject.getString("gas_level"));
		// Network Camera
		devObject = (JSONObject) devices.get(6);
		deviceStatus[6] = new String("Status: " + devObject.getString("status")
				+ "\nPort: " + devObject.getString("port"));
	}

	/**
	 * Get Command List JSON File From Server
	 * 
	 * @throws IOException
	 * @throws MalformedURLException
	 * @throws JSONException
	 */
	public void getCommandsList() throws IOException, MalformedURLException,
			JSONException {
		// Connect to Intel Galileo get Commands List
		HttpURLConnection httpCon = (HttpURLConnection) commandsJSONUrl
				.openConnection();
		httpCon.setReadTimeout(10000);
		httpCon.setConnectTimeout(15000);
		httpCon.setRequestMethod("GET");
		httpCon.setDoInput(true);
		httpCon.connect();
		// Read JSON File as InputStream
		InputStream readCommand = httpCon.getInputStream();
		Scanner scanCommand = new Scanner(readCommand).useDelimiter("\\A");
		// Set stream to String
		String commandFile = scanCommand.hasNext() ? scanCommand.next() : "";
		// Initialize serveFile as read string
		JSONObject commandsList = new JSONObject(commandFile);
		JSONObject temp = (JSONObject) commandsList.get("commands");
		JSONArray comArray = (JSONArray) temp.getJSONArray("command");
		int numberOfCommands = comArray.length();
		commands = new String[numberOfCommands];
		// Fill the Array
		for (int i = 0; i < numberOfCommands; i++) {
			JSONObject commandObject = (JSONObject) comArray.get(i);
			commands[i] = commandObject.getString("text");
		}
		Log.d("JSON", "Loaded " + commands[2]);
		httpCon.disconnect();
	}

	/**
	 * Fill the ListView Adapter
	 */
	public void fillListAdapter() {
		int numberOfItems = deviceStatus.length;
		for (int i = 0; i < numberOfItems; i++) {
			DeviceItem newItem = new DeviceItem();
			newItem.setDeviceName(deviceNames[i]);
			newItem.setDeviceStatus(deviceStatus[i]);
			newItem.setDeviceImage(deviceImages[i]);
			listItems.add(newItem);
		}
	}

	/**
	 * Write Command to Intel Galileo Socket
	 * 
	 * @param message
	 */
	public void writeToSocket(String message) {
		try {
			PrintWriter out = new PrintWriter(new BufferedWriter(
					new OutputStreamWriter(galileSocket.getOutputStream())),
					true);
			out.println(message);
			Log.d("SOCKET", "Message " + message + " Sent to Socket");
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	/**
	 * Update View Items
	 * 
	 * @param v
	 */
	public void onUpdate(View v) {
		updateDateText.invalidate();
		deviceListView.invalidateViews();
		deviceListView.refreshDrawableState();
		updateDateText.setText(lastUpdateDate);
		updateDateText.refreshDrawableState();
		Log.d("JSON", "Updated Views....");
	}
}
