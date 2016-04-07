var CANVAS_WIDTH = 1024;
var CANVAS_HEIGHT = 768;

$(document).ready(function(){
	var canvas = document.getElementById('myCanvas');  //store canvas outside event loop
	var ctx = canvas.getContext("2d");
	var x1=CANVAS_WIDTH/2, y1=CANVAS_HEIGHT/2;
	var x2, y2;     
	//append origin
	var coordListCanvas = [{'x':CANVAS_WIDTH/2, 'y':CANVAS_HEIGHT/2, 'z':5}];
	var coordListRelative = [{'x':0, 'y':0, 'z':5}];
	var speed = 1;
  var tangoSocket = io.connect('http://10.34.165.157:5000/pose');
  console.log('Connection to pose channel established...')



  tangoSocket.on('pose_current_ack', function(position) {
      
 
  		canvasPos = relativeCoordToCanvas(position['x']*100, position['y']*100, position['z'])
      var drone = new Image();
	    drone.src=document.getElementById('drone').src;

      // draw
      ctx.clearRect(0, 0, canvas.width, canvas.height);
      ctx.save();
			console.log(canvasPos);
			ctx.drawImage(drone, canvasPos['x'], canvasPos['y'],80,80);
			ctx.stroke();
  });



	function canvasCoordToRelative(x,y,z){
		return {'x': x - CANVAS_WIDTH/2, 'y': CANVAS_HEIGHT/2 - y, 'z':z}
	}

	function relativeCoordToCanvas(x,y,z){
		return {'x': x + CANVAS_WIDTH/2, 'y': CANVAS_HEIGHT/2 + y, 'z':z}
	}


	// when mouse button is clicked and held    
	$('#myCanvas').on('click', function(e){
	  var pos = getMousePos(canvas, e);
	  pos['z'] = 5;
	  coordListCanvas.push(pos);
	  coordListRelative.push(canvasCoordToRelative(pos['x'], pos['y'], pos['z']));
	  if(x2 === undefined){
			x2 = pos.x;
	  	y2 = pos.y;
	  }
	  else{
	  	x1 = x2;
	  	y1 = y2;
	  	x2 = pos.x;
	  	y2 = pos.y;
	  }
	  drawLineOnCanvas(canvas,x1,x2,y1,y2);
	});

	// get mouse pos relative to canvas (yours is fine, this is just different)
	function getMousePos(canvas, evt) {
	  var rect = canvas.getBoundingClientRect();
	  return {
	      x: evt.clientX - rect.left,
	      y: evt.clientY - rect.top
	  };
	}

	function drawLineOnCanvas(canvas, x1, x2, y1, y2){
		console.log(y1);
		console.log(y2);
		if(x1 !== undefined && x2 !== undefined){
			ctx.beginPath();
			ctx.moveTo(x1,y1);
			ctx.lineTo(x2,y2);
			ctx.strokeStyle="#FF0000";
			ctx.stroke();
			console.log(coordListCanvas);
		}
	}
	$('#completePath').click(function(){
		first = coordListCanvas[0];
		last = coordListCanvas[coordListCanvas.length - 1];
		drawLineOnCanvas(canvas, last.x, first.x, last.y, first.y);
		console.log(coordListRelative);

		path = {"path":coordListRelative};
		$.ajax({
    	type: 'POST',
    	contentType: 'application/json;charset=UTF-8',
    	url: '../pose/path_config',
    	data: JSON.stringify(path, null, '\t'),
    	success: function(msg){
        alert('wow' + msg);
    	}
		});
	});
});

