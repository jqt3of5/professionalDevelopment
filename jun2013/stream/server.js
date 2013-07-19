var net = require('net');
var spawn = require('child_process').spawn;
var readline = require('readline');
var sys = require("sys"),
    ws = require("./ws");


var capture = spawn('../hwi/capture');
var rlInput = readline.createInterface({
    input: capture.stdout, 
    output: capture.stdin
});


ws.createServer(function (websocket) {
    websocket.addListener("connect", function (resource) { 
      // emitted after handshake
      sys.debug("connect: " + resource);

      // server closes connection after 10s, will also get "close" event
      setTimeout(websocket.end, 10 * 1000); 
    }).addListener("data", function (data) { 
      // handle incoming data
      sys.debug(data);

      // send data to client
      rlInput.on('line', function(line) {
	websocket.write(line);
	});
      //websocket.write(JSON.stringify(out));
    }).addListener("close", function () { 
      // emitted when server or client closes connection
      sys.debug("close");
    });
  }).listen(8080);

//var server = net.createServer(function(con) {
//    console.log("A client has connected");

//    rlInput.on('line', function(line) {
//	con.write(line);
//    });
    // capture.stdout.on('data', function (data) {
    // 	con.write(data);
    // });

    // process.stdin.resume();
    // process.stdin.on('data', function(chunk) {
    // 	con.write(chunk);

    // });

    // process.stdin.on('end', function(chunk) {
    // 	process.stdout.write("end");
    // 	con.write("end");
    // });
//});

//server.listen(1234, function() {});

