from picamera2 import Picamera2, Preview
import time
import os
import sys

#Logging
log_file_path = "/home/pi/hypso3-camera-bachelor/picamtests/image_info_log.txt"
original_stdout = sys.stdout

#Camera config
cam = Picamera2()
directory = "/home/pi/Pictures/test"
num_pictures = 10
name : str = "/home/pi/Pictures/test/image{:03d}.jpg"
resolution = (4056, 3040)
mode = "still"
comment = ""

still_config = cam.create_still_configuration({"size": resolution})



cam.configure(still_config)

#Capture images
cam.start(show_preview = False)
starttime = time.time()


for i in range(num_pictures):
    cam.capture_file(name.format(i))
    if i == num_pictures - 1:
        break

endtime = time.time()

cam.stop()
totime = endtime - starttime

#Calculate filesizes
size = 0
count = 0
for i in range(num_pictures):
    filepath = name.format(i)
    
    if not os.path.islink(filepath):
        size += os.path.getsize(filepath)
        count +=1
        if count >= num_pictures:
            break

#Log results
msg = (f"Frames: {num_pictures}, Seconds: {totime:.2f}, FPS: {(num_pictures/totime):.2f}, Mbps: {((size*8/1e6)/totime):.2f}\n")
msg += (f"At Resolution: {resolution}, Avg. filesize: {size/num_pictures}, Mode: {mode}, Comment: {comment}")

print(msg)

with open(log_file_path, "a") as log_file:
    sys.stdout = log_file
    print(msg, file=log_file)
    sys.stdout = original_stdout

