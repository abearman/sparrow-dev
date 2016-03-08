#!/usr/bin/python
from sys import argv
import zbar
import cv2
from PIL import Image

# grab the reference to the camera
camera = cv2.VideoCapture(0)

# create a reader
scanner = zbar.ImageScanner()

# configure the reader
scanner.parse_config('enable')

# keep looping over the frames
while True:
	# grab the current frame
	(grabbed, frame) = camera.read()

	# check to see if we have reached the end of the video
	if not grabbed:
		break

	# show the frame and record if the user presses a key
	cv2.imshow("Frame", frame)

	# obtain image data
	pil = Image.fromarray(frame).convert('L')
	width, height = pil.size
	raw = pil.tostring()

	# wrap image data
	image = zbar.Image(width, height, 'Y800', raw)

	# scan the image for barcodes
	scanner.scan(image)

	# extract results
	#for symbol in image:
    # do something useful with results
	#	print 'decoded', symbol.type, 'symbol', '"%s"' % symbol.data
