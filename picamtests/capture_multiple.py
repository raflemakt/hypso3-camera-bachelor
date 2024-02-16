from picamera2 import Picamera2, Preview
import time
import os
import sys
from numpy import savetxt, save

#Logging
log_file_path = "/home/pi/hypso3-camera-bachelor/picamtests/image_info_log.txt"
raw_file_path = "/home/pi/hypso3-camera-bachelor/picamtests/pics/raw_file.txt"
original_stdout = sys.stdout

#Camera config
cam = Picamera2()
directory = "pics"
num_pictures = 30
name : str = "pics/image{:03d}.npy"
resolution = (4056, 3040)
mode = "raw"
comment = ""



cam_modes = cam.sensor_modes
cam_mode = cam_modes[3] 

jpg_config = cam.create_still_configuration({"size": resolution})
dng_config = cam.create_still_configuration(raw = {'format': 'SRGGB12'}, sensor = {'output_size': cam_mode['size'], 'bit_depth': cam_mode['bit_depth']})

cam.configure(dng_config) #change this
print(cam.camera_configuration()['raw'])

#Capture images
cam.start(show_preview = False)
time.sleep(1)
starttime = time.time()


for i in range(num_pictures):
    #cam.capture_file(name.format(i), name = "raw")
    
    raw = cam.capture_array("raw")
    #save(name.format(i), raw)
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
msg += (f"At Resolution: {cam_mode['size']}, Avg. filesize: {size/num_pictures}, Mode: {mode}, Comment: {comment}\n")

print(msg)

with open(log_file_path, "a") as log_file:
    sys.stdout = log_file
    print(msg, file=log_file)
    sys.stdout = original_stdout


