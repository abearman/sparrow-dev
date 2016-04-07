# USAGE
# python detect_barcode.py
# python detect_barcode.py --video video/barcode_example.mov

# import the necessary packages
import cv2
import zbar
from PIL import Image

camera = cv2.VideoCapture(0)


# keep looping over the frames
while True:
	# grab the current frame
	(grabbed, frame) = camera.read()
 
	# check to see if we have reached the end of the video
	if not grabbed:
		break

	# show the frame and record if the user presses a key
	cv2.imshow("Frame", frame)
	key = cv2.waitKey(0) & 0xFF

	scanner = zbar.ImageScanner()
	scanner.parse_config('enable')
		
	barcode_image = Image.fromarray(frame).convert('L')
	width, height = barcode_image.size
	raw = barcode_image.tostring()
	final_image = zbar.Image(width, height, 'Y800', raw)
	scanner.scan(final_image)
	
	for symbol in final_image.symbols:
		print 'decoded', symbol.type, 'symbol', '"%s"' % symbol.data


	# if the 'q' key is pressed, stop the loop
	if key == ord("q"):
		break
	


# cleanup the camera and close any open windows
camera.release()
cv2.destroyAllWindows()
