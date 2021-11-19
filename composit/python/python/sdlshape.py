#import necessary modules
import cv2
import sdl2
import sdl2.ext
import numpy

windowSize = (640,480)

#initialize the camera
vc = cv2.VideoCapture(0)
vc.set(cv2.cv.CV_CAP_PROP_FRAME_WIDTH, windowSize[0])
vc.set(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT, windowSize[1])

#grab and show a first frame from the camera
junk,image = vc.read()
cv2.imshow('0',image)

#initialize sdl2
sdl2.ext.init()
window = sdl2.ext.Window("test", size=windowSize)
window.show()
windowSurf = sdl2.SDL_GetWindowSurface(window.window)
windowArray = sdl2.ext.pixels3d(windowSurf.contents)

while True: #keep reading to have a live feed from the cam
    junk,image = vc.read()
    image = numpy.insert(image,3,255,axis=2) #add alpha
    image = numpy.rot90(image) #rotate dims
    numpy.copyto(windowArray, image)
    window.refresh()
