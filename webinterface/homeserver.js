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

/*
 * Required NodeJS Modules to Include...
 */
var http = require('http');
var path = require('path');
var fs = require('fs');
var url = require('url');
var exec = require('child_process').exec;
var child;
/*
 * Objects and variables
 */
var mimeTypes = {
		'.json' : 'text/json',
		'.css' : 'text/css',
		'.gif' : 'image/gif',
		'.png' : 'image/png',
		'jpg'  : 'image/jpg'
	};
var jsonFilePath = "/home/root/smarthome/home.json";
var commandsFilePath = "/home/root/smarthome/commands.json";
var pipePath = "/home/root/smarthome/SMARTHOMEPIPE";
var resourcePath = "res/";
//Server Listen Port
var PORT = 3000;

//Create Server Object and Callback function handler
var app = http.createServer(handler).listen(PORT);

//Parse JSON File for Home Status
var homestatus;
fs.readFile(jsonFilePath, 'utf8', function (err, data) {
	  if (err) throw err;
	  homestatus = JSON.parse(data);
});
/**
 * Create HTML Response for UI Request
 * 
 * @param res
 * @returns
 */
function JSONtoHtmlUI(res,message) {
	/**
	 * Create HTML UI with jQuery List View
	 */
	res.writeHead({'Content-Type': 'text/html'});
	res.write('<!DOCTYPE html>');
	res.write('<html>');
	res.write('<head>');
	res.write('<meta charset="UTF-8">');
	res.write('<meta name="viewport" content="width=device-width, initial-scale=1">');
	res.write('<link rel="stylesheet" href="http://code.jquery.com/mobile/1.4.5/jquery.mobile-1.4.5.min.css">');
	res.write('<script src="http://code.jquery.com/jquery-1.11.1.min.js"></script>');
	res.write('<script src="http://code.jquery.com/mobile/1.4.5/jquery.mobile-1.4.5.min.js"></script>');
	res.write('</head>');
	res.write('<body>');
	res.write('<div data-role="page" id="pageone">');
	res.write('<div data-role="main" class="ui-content">');
	if(message != ""){
		res.write('<p> Message: ' + message + '</p>');
	}
	res.write('<h4> Home Status: ' + homestatus["home"]["last_update_date"] + '</h4>');
	res.write('<ul data-role="listview" data-inset="true">');
	// List Devices
	// SHT11 Sensor
	res.write('<li>');
	res.write('<a href=/>');
	res.write('<img src="'+ resourcePath + 'sht11.png">');
	res.write('<h3>' + homestatus["home"]["device"][0].name + '</h3>');
	res.write('<p> Status: ' + homestatus["home"]["device"][0].status + ', TEMPERATURE: ' + homestatus["home"]["device"][0].temperature + ' Celcius, HUMIDITY: ' + homestatus["home"]["device"][0].humidity + '</p></a></li>' );
	// Philio Multi Sensor
	res.write('<li>');
	res.write('<a href=/>');
	res.write('<img src="'+ resourcePath + 'philio.jpg">');
	res.write('<h3>' + homestatus["home"]["device"][1].name + '</h3>');
	res.write('<p> Status: ' + homestatus["home"]["device"][1].status + ', TEMPERATURE: ' + homestatus["home"]["device"][1].temperature + ' Fahreneit, LUMINATION: ' + homestatus["home"]["device"][1].lumination + '</p>' );
	res.write('<p> MOTION is: ' + homestatus["home"]["device"][1].motion + ', DOOR/WINDOW: ' + homestatus["home"]["device"][1].door + ' Battery: ' + homestatus["home"]["device"][1].battery + '% </p></a></li>' );
	// Fibaro Wall Plug
	res.write('<li>');
	res.write('<a href=switch3>');
	res.write('<img src="'+ resourcePath + 'fibaro.png">');
	res.write('<h3>' + homestatus["home"]["device"][2].name + '</h3>');
	res.write('<p> Status: ' + homestatus["home"]["device"][2].status + '</p>' );
	res.write('<p><strong> SWITCH is : ' + homestatus["home"]["device"][2].switch + '</strong></p>' );
	res.write('<p> Current Power Consumption: ' + homestatus["home"]["device"][2].power_meter + ' kWh, Cumulative Energy Consumption: ' + homestatus["home"]["device"][2].energy_meter + ' kWh </p></a></li>' );
	res.write('<li>');
	//Everspring Lamp Holder
	res.write('<a href=switch4>');
	res.write('<img src="'+ resourcePath + 'everspring.png">');
	res.write('<h3>' + homestatus["home"]["device"][3].name + '</h3>');
	res.write('<p> Status: ' + homestatus["home"]["device"][3].status + '</p>' );
	res.write('<p><strong> SWITCH is : ' + homestatus["home"]["device"][3].switch + '</strong></p>' );
	res.write('</a></li>' );
	res.write('<li>');
	//Flood Detector
	res.write('<a href=/>');
	res.write('<img src="'+ resourcePath + 'flood.jpg">');
	res.write('<h3>' + homestatus["home"]["device"][4].name + '</h3>');
	res.write('<p> Status: ' + homestatus["home"]["device"][4].status + '</p>' );
	res.write('<p> FLOOD is : ' + homestatus["home"]["device"][4].flood + '</p>' );
	res.write('</a></li>' );
	res.write('<li>');
	//MQ-9 Sensor
	res.write('<a href=/>');
	res.write('<img src="'+ resourcePath + 'mq9.png">');
	res.write('<h3>' + homestatus["home"]["device"][5].name + '</h3>');
	res.write('<p> Status: ' + homestatus["home"]["device"][5].status + '</p>' );
	res.write('<p>GAS LEVEL is : ' + homestatus["home"]["device"][5].gas_level + '</p>' );
	res.write('</a></li>' );
	res.write('<li>');
	//D-Link Network Camera
	res.write('<a href=capture>');
	res.write('<img src="'+ resourcePath + 'dlink.png">');
	res.write('<h3>' + homestatus["home"]["device"][6].name + '</h3>');
	res.write('<p> Status: ' + homestatus["home"]["device"][6].status + '</p>' );
	res.write('</a></li>' );
	res.write('</ul>');
	res.write('</div>');
	res.write('</div>'); 
	res.write('</body>');
	res.write('</html>');
    res.end();
}

/**
 * Send Command to Smart Home Application Pipeline
 * @param command_id
 * @returns
 */
function sendCommand(command_id) {
	// Parse Commands JSON
	var commands = JSON.parse(fs.readFileSync(commandsFilePath, 'utf8'));
	console.log("Parsing Commands JSON...");
	var command = 'echo ' + commands["commands"]["command"][command_id].text + ' > ' + pipePath;
	console.log("Executing....:" + command);
	child = exec(command, function(error, stdout, stderr) {
		if (error != null) {
			console.log('exec error: ' + error);
		}
	});
}
/**
 * HTTP Server Request Handler Callback Function
 * @param request
 * @param response
 * @returns
 */
function handler(request, response) {
	if (request.method !== 'GET') {
		response.writeHead(405);
		response.end('Unsupported request method', 'utf8');
		return;
	}
	console.log("Parsing Device File...")
	
	fs.readFile(jsonFilePath, 'utf8', function (err, data) {
		  if (err) throw err;
		  homestatus = JSON.parse(data);
	});
	var request = url.parse(request.url, true);
	var action = request.pathname;
	// Consolo Output for Request
	console.log(action);
	/**
	 * Handle HTTP Get Requests
	 */
	if (action === '/'){
		JSONtoHtmlUI(response,"");
		return;
	}else if(action == '/switch4'){
		console.log("Switch Request....");
		var switch_action;
		// Switch off
		if(homestatus["home"]["device"][3].switch == "ON"){
			switch_action = "OFF";
			sendCommand(7);
		}else { // Switch on wall plug
			switch_action = "ON";
			sendCommand(6)
		}
		homestatus["home"]["device"][3].switch = switch_action;
		JSONtoHtmlUI(response,"Switched " + homestatus["home"]["device"][3].name + " " + switch_action);
		return this;
		// Switch On or Off LampHolder
	}else if(action == '/switch3'){
		console.log("Switch Request.....");
		var switch_action;
		if(homestatus["home"]["device"][2].switch == "ON"){
			switch_action = "OFF";
			sendCommand(5);
		}else{
			switch_action = "ON";
			sendCommand(4);
		}
		homestatus["home"]["device"][2].switch = switch_action;
		JSONtoHtmlUI(response,"Switched " + homestatus["home"]["device"][2].name + " " + switch_action);
		return this;
		// Capture Frame from Network Camera
	}else if(action == '/capture'){
		sendCommand(8);
		JSONtoHtmlUI(response,"Captured From Network Camera");
		return this;
	}
	/**
	 * Serve Requested Static Files
	 */
	var filePath = path.join(__dirname, action);
	var stat = fs.statSync(filePath);
	
	fs.exists(filePath, function (exists) {
	   if (!exists) {
	       // 404 missing files
	       response.writeHead(404, {'Content-Type': 'text/plain' });
	       response.end('404 Not Found');
	       return;
	    }
	    var readStream = fs.createReadStream(filePath);
	    readStream.on('data',function(data){
	    	response.write(data);
	    });
	    readStream.on('end',function(data){
	    	response.end();
	    });
	});
	return;
}
