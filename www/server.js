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

        //io.emit('onmidi', { a: '1', c: '0', n: '66', v: '80' });
        //socket.broadcast.emit('onmidi', { hello2: 'a user emits' });

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



        //socket.emit('onmidi', { midi: 'tra la la' });
        //socket.broadcast.emit('onmidi', 'hello friends!');

        //console.log('onmidi---');

        //var arr = data.split('#');


        //var a = arr[0];	// Action eg  NoteOn = 1, NoteOff = 0
        //var c = arr[1];	// Midi Channel
        //var n = arr[2]; // Note
        //var v = arr[3];	// Velocity

        //socket.broadcast.emit('onmidi', { a: a, c: c, n: n,v: v });

        //var midiobj = {a:a, c:c, n:n, v:v};

        //socket.broadcast.emit('onmidi', 'hello friends!');
        //socket.emit('onmidi', { midi: 'tra la la' });
        //console.log(data);

    });


});

http.listen(port, function () {
    console.log('listening on *:3000');
});
