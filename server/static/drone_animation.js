
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