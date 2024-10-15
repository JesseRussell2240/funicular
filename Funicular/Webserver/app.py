from flask import Flask, render_template, Response, url_for
import time

app = Flask(__name__)

@app.route('/')
def index():
    return render_template('index.html')

# Placeholder for video feed (simulated or real camera feed)
@app.route('/video_feed')
def video_feed():
    # Use this function to stream the live video feed from your car
    return "Video feed placeholder"

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
