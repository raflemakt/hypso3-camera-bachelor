from picamera2 import Picamera2, Preview
import time
import os

cam = Picamera2()
directory = "/home/pi/Pictures/test"
num_pictures = 10
name : str = "/home/pi/Pictures/test/image{:03d}.jpg"
resolution = (800, 600)
mode = "video"

cam.still_configuration.size = resolution
cam.video_configuration.size = resolution


cam.configure(mode)
cam.start(show_preview = False)

starttime = time.time()


for i in range(num_pictures):
    cam.capture_file(name.format(i))
    if i == num_pictures - 1:
        break

endtime = time.time()

cam.stop()
totime = endtime - starttime


size = 0
count = 0
for i in range(num_pictures):
    filepath = name.format(i)
    
    if not os.path.islink(filepath):
        print(os.path.getsize(filepath))
        size += os.path.getsize(filepath)
        count +=1
        if count >= num_pictures:
            break
    
print(f"Frames: {num_pictures}, Seconds: {totime}, FPS: {num_pictures/totime}, Mbps: {(size*8/1e6)/totime}")
print(f"At Resolution: {resolution}, Avg. filesize: {size/num_pictures}, Mode: {mode}")