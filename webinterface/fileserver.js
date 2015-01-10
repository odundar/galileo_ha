var http = require('http'),
    fs = require('fs'),
    path = require('path');

http.createServer(function(request, response) {
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
})
.listen(2000);
