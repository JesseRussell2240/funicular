# main.py

import cv2
import time
from camera_setup import CameraSetup
from capture_frames import FrameCapture
from Perception.color_detection import ColorDetection
from queue import Queue, Empty

def main():
    # Initialize camera
    camera = CameraSetup(resolution=(640, 480), framerate=30)
    
    # Create a queue for frames
    frame_queue = Queue(maxsize=10)
    
    # Initialize frame capture
    frame_capture = FrameCapture(camera_setup=camera, frame_queue=frame_queue, queue_size=10)
    frame_capture.start()
    
    # Initialize perception module
    color_detector = ColorDetection()
    
    try:
        while True:
            try:
                frame = frame_queue.get(timeout=1)
                
                # Process frame for color detection
                detected_cones = color_detector.detect_cones(frame)
                
                # Implement further decision making and control logic here
                
                # Display frame with detections (optional)
                cv2.imshow('Processed Frame', detected_cones)
                
                if cv2.waitKey(1) & 0xFF == ord('q'):
                    break
            except Empty:
                print("No frame available.")
                continue
    except KeyboardInterrupt:
        print("Interrupted by user.")
    finally:
        frame_capture.stop()
        cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
