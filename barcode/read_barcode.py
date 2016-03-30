#### Code for BARCODE detection  ######
import cv,sys
imgco = cv.LoadImage('image.jpg')
img = cv.CreateImage(cv.GetSize(imgco),8,1)
imgx = cv.CreateImage(cv.GetSize(img),cv.IPL_DEPTH_16S,1)
imgy = cv.CreateImage(cv.GetSize(img),cv.IPL_DEPTH_16S,1)
thresh = cv.CreateImage(cv.GetSize(img),8,1)

### Convert image to grayscale ###
cv.CvtColor(imgco,img,cv.CV_BGR2GRAY)

### Finding horizontal and vertical gradient ###

cv.Sobel(img,imgx,1,0,3)
cv.Abs(imgx,imgx)

cv.Sobel(img,imgy,0,1,3)
cv.Abs(imgy,imgy)

cv.Sub(imgx,imgy,imgx)
cv.ConvertScale(imgx,img)

### Low pass filtering ###
cv.Smooth(img,img,cv.CV_GAUSSIAN,7,7,0)

### Applying Threshold ###
cv.Threshold(img,thresh,100,255,cv.CV_THRESH_BINARY)

cv.Erode(thresh,thresh,None,2)
cv.Dilate(thresh,thresh,None,5)

### Contour finding with max. area ###
storage = cv.CreateMemStorage(0)
contour = cv.FindContours(thresh, storage, cv.CV_RETR_CCOMP, cv.CV_CHAIN_APPROX_SIMPLE)
area = 0
while contour:
    max_area = cv.ContourArea(contour)
    if max_area>area:
        area = max_area
        bar = list(contour) 
    contour=contour.h_next()

### Draw bounding rectangles ###
bound_rect = cv.BoundingRect(bar)
pt1 = (bound_rect[0], bound_rect[1])
pt2 = (bound_rect[0] + bound_rect[2], bound_rect[1] + bound_rect[3])
cv.Rectangle(imgco, pt1, pt2, cv.CV_RGB(0,255,255), 2)

cv.ShowImage('img',imgco)    
cv.SaveImage("box.jpg", imgco)
cv.WaitKey(0)  
