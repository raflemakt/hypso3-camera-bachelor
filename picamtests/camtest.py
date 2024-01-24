from picamera import PiCamera
import time
import io
import os

camera = PiCamera()

# Set resolution and framerate
camera.resolution = (1920, 1080)
camera.framerate = 30

# Create a stream to capture images
stream = io.BytesIO()

starttime = time.time()
frames = 30

file_paths = []
file_sizes = []

# Capture and save images using capture_continuous
for i, foo in enumerate(camera.capture_continuous(stream, format="jpeg", use_video_port=True)):
    # Ensure the stream is at its beginning
    stream.seek(0)

    # Save the image to a file
    file_path = f'/home/mikked2101/Pictures/image{i}.jpg'
    with open(file_path, 'wb') as file:
        file.write(stream.read())

    # Reset the stream for the next capture
    file_paths.append(file_path)
    file_size = os.path.getsize(file_path)
    file_sizes.append(file_size)
    
    stream.seek(0)
    stream.truncate()

    if i + 1 >= frames:
        break

endtime = time.time()
totime = endtime - starttime
FPS = frames / totime
totbits = 0;
for filesize in file_sizes:
    totbits += filesize
bitrate = totbits /totime
    
print(f'Frames: {frames}, Seconds: {totime}, FPS: {FPS}')
