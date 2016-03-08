import sys
import zbar
from PIL import Image

def read_barcode(image_path):
	# create a reader
	scanner = zbar.ImageScanner()

	# configure the reader
	scanner.parse_config('enable')

	# obtain image data
	pil = Image.open(image_path).convert('L')
	width, height = pil.size
	raw = pil.tostring()

	# wrap image data
	image = zbar.Image(width, height, 'Y800', raw)

	# scan the image for barcodes
	scanner.scan(image)

	# extract results
	for symbol in image:
	    # do something useful with results
  	  print 'decoded', symbol.type, 'symbol', '"%s"' % symbol.data


if __name__ == '__main__':
	read_barcode(sys.argv[1])
