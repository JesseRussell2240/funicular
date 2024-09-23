# test_camera_module.py

import cv2
import time
from camera_setup import CameraSetup
from capture_frames import FrameCapture
from queue import Queue, Empty

def main():
    # Initialize camera setup
    camera = CameraSetup(resolution=(640, 480), framerate=30)
    
    # Create a queue to hold frames
    frame_queue = Queue(maxsize=10)
    
    # Initialize frame capture
    frame_capture = FrameCapture(camera_setup=camera, frame_queue=frame_queue, queue_size=10)
    frame_capture.start()
    
    try:
        while True:
            try:
                # Retrieve frame from the queue
                frame = frame_queue.get(timeout=1)
                
                # Display the frame
                cv2.imshow('Camera Feed', frame)
                
                # Break the loop on 'q' key press
                if cv2.waitKey(1) & 0xFF == ord('q'):
                    break
            except Empty:
                print("No frame available.")
                continue
    except KeyboardInterrupt:
        print("Interrupted by user.")
    finally:
        # Stop frame capturing
        frame_capture.stop()
        cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
