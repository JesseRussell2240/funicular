from flask import Flask, render_template, Response
from picamera2 import Picamera2
from io import BytesIO
from time import sleep

app = Flask(__name__)

# Initialize the camera
picam2 = Picamera2()
picam2.configure(picam2.create_video_configuration(main={"size": (640, 480)}))
picam2.start()

def generate_frames():
    while True:
        # Capture a frame from the camera
        frame = picam2.capture_array()
        
        # Convert the frame to a JPEG image
        stream = BytesIO()
        picam2.capture_file(stream, format='jpeg')
        stream.seek(0)
        
        # Yield the frame in a byte stream
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + stream.read() + b'\r\n')

@app.route('/')
def index():
    return render_template('index.html')

# Video streaming route
@app.route('/video_feed')
def video_feed():
    return Response(generate_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=False)
