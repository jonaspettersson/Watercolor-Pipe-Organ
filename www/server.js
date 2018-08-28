var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var fs = require('fs');


var port = process.env.PORT || 1337;

app.get('/', function (req, res) {
    res.sendFile(__dirname + '/app.html');
});


io.on('connection', function (socket) {

    io.emit('news', { hello: 'a user just connected' });
    //io.emit('onmidi', { a: '1', c: '0', n: '66', v: '80' });

    socket.on('myevent', function (data) {
        console.log(data);
    });
    socket.on('getdata', function (data) {
        io.emit('getdata', { oruk: 'test' });
    });


    socket.on('midi', function (data) {


		try {
		
			var a = data.a;
			var c = data.c;
			var n = data.n;
			var v = data.v;
					
			var str = '{ "a": '+a+', "c": '+c+', "n": '+n+', "v": '+v+' }';
			var obj = JSON.parse(str);		
			
			socket.broadcast.emit('onmidi', { a: a, c: c, n: n, v: v });
			socket.broadcast.emit('midijson', obj);
		
		  } catch (error) {
			  
			console.error(error);
		}


    });


});

http.listen(port, function () {
    console.log('listening on *:3000');
});
