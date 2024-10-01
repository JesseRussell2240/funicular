from flask import Flask, render_template, request
import socket
#import cv2  # Comment this out if it's related to camera functionality

app = Flask(__name__)

# Commented out for testing without camera
#cap = cv2.VideoCapture(0)

@app.route('/', methods=['GET', 'POST'])
def index():
    result = ''
    if request.method == 'POST':
        command = request.form['command']

        # You can replace this part with whatever action you're testing
        result = execute_command(command)  # Function to execute command and return result

    return render_template('index.html', result=result)

# Example command execution function
def execute_command(command):
    # Perform socket communication or another action
    if command == "forward":
        return "Moving forward"
    elif command == "back":
        return "Moving back"
    else:
        return "Unknown command"

if __name__ == '__main__':
    # Run the Flask web server on port 8080
    app.run(debug=True, host='0.0.0.0', port=8080)
