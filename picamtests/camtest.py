from picamera import PiCamera
import time

camera = PiCamera()

starttime = time.time()
frames = 10
for i in range(frames):
    camera.capture('/home/mikked2101/Pictures/image%s.jpg' % i)
endtime = time.time()
totime = endtime-starttime
FPS = frames/totime
print(f'Frames: {frames}, Seconds: {totime}, FPS: {FPS}')

      