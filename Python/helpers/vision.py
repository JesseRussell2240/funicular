from picamera2 import Picamera2
from io import BytesIO
import cv2
import numpy as np

picam2 = Picamera2()
picam2.configure(picam2.create_video_configuration(main={"size": (640, 480)}))
picam2.start()

def detect_pylons(frame):
    # Your existing detection logic here
    pass

def generate_frames():
    while True:
        frame = picam2.capture_array()
        frame = detect_pylons(frame)
        stream = BytesIO()
        success, encoded_image = cv2.imencode('.jpeg', frame)
        if success:
            stream.write(encoded_image.tobytes())
            stream.seek(0)
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + stream.read() + b'\r\n')
