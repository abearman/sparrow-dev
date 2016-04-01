$(document).ready(function(){
	var canvas = document.getElementById('myCanvas');  //store canvas outside event loop
	var ctx = canvas.getContext("2d");
	var x1, y1, x2, y2;     //to store the coords
	var coordList = [];
	var speed = 1;


	// when mouse button is clicked and held    
	$('#myCanvas').on('click', function(e){
	  var pos = getMousePos(canvas, e);
	  pos['z'] = 5;
	  coordList.push(pos);
	  if(x1 === undefined){
	  	//first click
	  	x1 = pos.x;
	  	y1 = pos.y;
	  }
	  else if(x2 === undefined){
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
		if(x1 !== undefined && x2 !== undefined){
			ctx.beginPath();
			ctx.moveTo(x1,y1);
			ctx.lineTo(x2,y2);
			ctx.strokeStyle="#FF0000";
			ctx.stroke();
			console.log(coordList);
		}
	}
	$('#completePath').click(function(){
		first = coordList[0];
		last = coordList[coordList.length - 1];
		drawLineOnCanvas(canvas, last.x, first.x, last.y, first.y);
		//make post request to server
		$.ajax({
    	type: 'POST',
    	url: 'http://localhost:5000/pose/path_config',
    	data: { 
    		coordList
    	},
    	success: function(msg){
        alert('wow' + msg);
    	}
		});
	});












	window.requestAnimFrame = (function (callback) {
	    return window.requestAnimationFrame || window.webkitRequestAnimationFrame || window.mozRequestAnimationFrame || window.oRequestAnimationFrame || window.msRequestAnimationFrame || function (callback) {
	        window.setTimeout(callback, 1000 / 60);
	    };
	})();


	function makePolyPoints(pathArray){
    var points=[];
    for(var i=1;i<pathArray.length;i++){
        var startPt=pathArray[i-1];
        var endPt=pathArray[i];
        var dx = endPt.x-startPt.x;
        var dy = endPt.y-startPt.y;
        for(var n=0;n<=200;n++){
            var x= startPt.x + dx * n/200;
            var y= startPt.y + dy * n/200;
            points.push({x:x,y:y});
        }
    }
    return(points);
	}
	var fps = 60;
	var width = 30;
	var height = 30;
	var position = 0;
	var speed = 2;
	// var drone = new Image();
	// drone.src="3dr_icon_square.png";
	// console.log(drone);



function animate() {
    setTimeout(function () {
    		var coordListCopy = coordList.concat([coordList[0]]);
    		var polypoints = makePolyPoints(coordListCopy);
        requestAnimFrame(animate);

        // calc new position
        position += speed;
        if (position > polypoints.length - 1) {
        		position = 0;
            return;
        }
        var pt = polypoints[position];
        	var drone = new Image();
	drone.src="3dr_icon_square.png";

        // draw
        ctx.clearRect(0, 0, canvas.width, canvas.height);
        ctx.save();
        for(var c=0; c<coordListCopy.length-1; c++ ){
        	drawLineOnCanvas(canvas, coordListCopy[c].x, coordListCopy[c+1].x, coordListCopy[c].y, coordListCopy[c+1].y);
        }
        ctx.beginPath();
        //ctx.translate(pt.x, pt.y);
        console.log(drone);
        ctx.drawImage(drone, pt.x-40, pt.y-40,80,80);
        return;
        //ctx.rect(-width / 2, -height / 2, 30, 30);
        //ctx.arc(pt.x, pt.y, 10, 0, 2 * Math.PI, false);
        ctx.fill();
        ctx.stroke();
        ctx.restore();

    }, 1000 / fps);
}
});