


'use strict';
var http = require('http');
var fs = require('fs');
var port = process.env.PORT || 1337;

http.createServer(function (req, res) {


    fs.readFile(__dirname + '/app.html',
        function (err, data) {
            if (err) {
                res.writeHead(500);
                return res.end('Error loading app.html');
            }

            res.writeHead(200);
            res.end(data);
        });

    //res.writeHead(200, { 'Content-Type': 'text/plain' });
    //res.end('Hello World\n');
}).listen(port);
