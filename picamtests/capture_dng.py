
import time

from picamera2 import Picamera2, Preview

picam2 = Picamera2()
#picam2.start_preview(Preview.QTGL)

def take_dng_pics(num_pics, file_output):

capture_config = picam2.create_still_configuration(raw={'format': 'SRGGB12'})
picam2.configure(capture_config)

picam2.start()
time.sleep(2)

picam2.capture_file("pics/gunnmry.dng", name="raw")
print(picam2.stream_configuration("raw"))
picam2.stop()

