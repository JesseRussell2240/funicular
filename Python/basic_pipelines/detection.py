import gi
gi.require_version('Gst', '1.0')
from gi.repository import Gst, GLib
import os
import numpy as np
import cv2
import hailo
import sys
import json
from hailo_rpi_common import (
    get_caps_from_pad,
    get_numpy_from_buffer,
    app_callback_class,
)
from detection_pipeline import GStreamerDetectionApp


# -----------------------------------------------------------------------------------------------
# User-defined class to be used in the callback function
# -----------------------------------------------------------------------------------------------
# Inheritance from the app_callback_class
class user_app_callback_class(app_callback_class):
    def __init__(self):
        super().__init__()
        self.detections = []  # List to store detections (size, color, coordinates)

    def new_function(self):  # New function example
        return "The meaning of life is: "

# -----------------------------------------------------------------------------------------------
# Helper function to classify cone color
# -----------------------------------------------------------------------------------------------
def classify_cone_color(frame, x1, y1, x2, y2):
    """Classifies the color of the cone based on the region in the frame."""
    region = frame[y1:y2, x1:x2]
    avg_color = cv2.mean(region)[:3]  # Get average BGR color

    # Simple thresholds for red and blue
    blue_threshold = avg_color[0] > avg_color[1] and avg_color[0] > avg_color[2]  # Blue > Green & Red
    red_threshold = avg_color[2] > avg_color[1] and avg_color[2] > avg_color[0]  # Red > Green & Blue

    if red_threshold:
        return 'red'
    elif blue_threshold:
        return 'blue'
    else:
        return 'unknown'


# -----------------------------------------------------------------------------------------------
# User-defined callback function
# -----------------------------------------------------------------------------------------------
def app_callback(pad, info, user_data):
    buffer = info.get_buffer()
    if buffer is None:
        return Gst.PadProbeReturn.OK

    # Increment frame count
    user_data.increment()

    # Get video frame
    format, width, height = get_caps_from_pad(pad)
    frame = None
    if user_data.use_frame and format is not None and width is not None and height is not None:
        frame = get_numpy_from_buffer(buffer, format, width, height)

    # Get detections from the buffer
    roi = hailo.get_roi_from_buffer(buffer)
    detections = roi.get_objects_typed(hailo.HAILO_DETECTION)

    print(f"Detections found: {len(detections)}")

    # Clear previous detections
    user_data.detections.clear()

    # Process detections
    for detection in detections:
        label = detection.get_label()
        bbox = detection.get_bbox()

        # Get normalized bounding box coordinates
        x1_norm = bbox.xmin()
        y1_norm = bbox.ymin()
        x2_norm = bbox.xmax()
        y2_norm = bbox.ymax()
        x1 = int(x1_norm * width)
        y1 = int(y1_norm * height)
        x2 = int(x2_norm * width)
        y2 = int(y2_norm * height)

        # Classify cone color
        color = classify_cone_color(frame, x1, y1, x2, y2)

        # Append detection to user_data
        user_data.detections.append((color, x1, y1, x2, y2, detection.get_confidence()))

    # Draw bounding boxes
    largest_red = None
    largest_blue = None

    # Find largest red and blue cones
    for detection in user_data.detections:
        color, x1, y1, x2, y2, confidence = detection
        area = (x2 - x1) * (y2 - y1)

        if color == 'red' and (largest_red is None or area > largest_red[0]):
            largest_red = (area, x1, y1, x2, y2)
        elif color == 'blue' and (largest_blue is None or area > largest_blue[0]):
            largest_blue = (area, x1, y1, x2, y2)

    # Draw all detections with green bounding boxes
    for detection in user_data.detections:
        color, x1, y1, x2, y2, confidence = detection
        cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 255, 0), 2)

    # Highlight largest cones with respective colors
    if largest_red is not None:
        _, x1, y1, x2, y2 = largest_red
        cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 0, 255), 2)  # Red box

    if largest_blue is not None:
        _, x1, y1, x2, y2 = largest_blue
        cv2.rectangle(frame, (x1, y1), (x2, y2), (255, 0, 0), 2)  # Blue box

    # Save the processed frame
    if user_data.use_frame and frame is not None:
        cv2.imwrite("frame.jpg", frame)

    # Save JSON output
    result = {
    "largest_red_cone": {
        "area": 0,
        "center": (0, 0),
        "vertical_height": 0,
        "bbox": (0, 0, 0, 0)
    },
    "largest_blue_cone": {
        "area": 0,
        "center": (0, 0),
        "vertical_height": 0,
        "bbox": (0, 0, 0, 0)
    }
}

    # Default values when no cones are detected
    red_center = (0, 0)
    blue_center = (0, 0)
    red_height = 0
    blue_height = 0

    # If largest_red is detected, update the details
    if largest_red:
        red_x1, red_y1, red_x2, red_y2 = largest_red[1], largest_red[2], largest_red[3], largest_red[4]
        red_center = ((red_x1 + red_x2) // 2, (red_y1 + red_y2) // 2)  # Center coordinates
        red_height = red_y2 - red_y1  # Vertical height in pixels
        result["largest_red_cone"] = {
            "area": largest_red[0],
            "center": red_center,
            "vertical_height": red_height,
            "bbox": (red_x1, red_y1, red_x2, red_y2)
        }

    # If largest_blue is detected, update the details
    if largest_blue:
        blue_x1, blue_y1, blue_x2, blue_y2 = largest_blue[1], largest_blue[2], largest_blue[3], largest_blue[4]
        blue_center = ((blue_x1 + blue_x2) // 2, (blue_y1 + blue_y2) // 2)  # Center coordinates
        blue_height = blue_y2 - blue_y1  # Vertical height in pixels
        result["largest_blue_cone"] = {
            "area": largest_blue[0],
            "center": blue_center,
            "vertical_height": blue_height,
            "bbox": (blue_x1, blue_y1, blue_x2, blue_y2)
        }

    # Write to JSON if there are any detections
    if result:
        with open("cone_detection.json", "w") as json_file:
            json.dump(result, json_file, indent=4)
        print("JSON written with cone detection results.")
    else:
        print("No cones detected. Writing default JSON.")

        # Convert frame to BGR for user_data
    if frame is not None:
        frame = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)
        user_data.set_frame(frame)

    print(f"Processed frame: {user_data.get_count()}")
    
        
    return Gst.PadProbeReturn.OK



if __name__ == "__main__":
    sys.argv = [
        "script.py",  # Name of the script (you can leave it as "script.py" or as whatever the script name is)
        "--hef-path", "/home/rpi/Desktop/Beta/Python/hailo-rpi5-examples/resources/funicular2.hef",
        "--labels-json", "/home/rpi/Desktop/Beta/Python/hailo-rpi5-examples/resources/funicular2.json",
        "--use-frame"  # Adding the --use-frame argument
    ]
    
    # Create an instance of the user app callback class
    user_data = user_app_callback_class()
    app = GStreamerDetectionApp(app_callback, user_data)
    app.run()
