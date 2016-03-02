import numpy as np
import cv2
import glob

# termination criteria
criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)

# prepare object points, like (0,0,0), (1,0,0), (2,0,0) ....,(6,5,0)
objp = np.zeros((6*7,3), np.float32)
objp[:,:2] = np.mgrid[0:7,0:6].T.reshape(-1,2)

# Arrays to store object points and image points from all the images.
objpoints = [] # 3d point in real world space
imgpoints = [] # 2d points in image plane.

images = glob.glob('*.jpg')
print images

for fname in images:
	img = cv2.imread(fname)
	print img.shape	
	img_height = img.shape[0]  # Num columns
	img_width = img.shape[1]   # Num rows
		
	gray = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)

