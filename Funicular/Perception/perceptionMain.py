# main.py

import cv2
import time
import logging
from camera_setup import CameraSetup
from capture_frames import FrameCapture
from perception.color_detection import ColorDetection
from perception.image_processing import ImageProcessing
from perception.vision_utils import VisionUtils
from queue import Queue, Empty

def load_config():
    """
    Load configuration parameters for color detection and image processing.
    
    Returns:
        dict: Configuration dictionary.
    """
    config = {
        'blue_lower': (100, 150, 50),    # HSV lower bound for blue
        'blue_upper': (140, 255, 255),   # HSV upper bound for blue
        'orange_lower': (10, 100, 20),   # HSV lower bound for orange
        'orange_upper': (25, 255, 255),  # HSV upper bound for orange
        'gaussian_kernel': (5, 5),
        'canny_threshold1': 50,
        'canny_threshold2': 150
    }
    return config

def main():
    # Setup logging for the main script
    logging.basicConfig(level=logging.DEBUG, 
                        format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    logger = logging.getLogger('Main')
    
    # Load configurations
    config = load_config()
    
    # Initialize camera setup
    camera = CameraSetup(resolution=(640, 480), framerate=30)
    
    # Create a queue to hold frames
    frame_queue = Queue(maxsize=10)
    
    # Initialize frame capture
    frame_capture = FrameCapture(camera_setup=camera, frame_queue=frame_queue, queue_size=10)
    frame_capture.start()
    
    # Initialize perception modules
    color_detector = ColorDetection(config)
    image_processor = ImageProcessing(config)
    vision_utils = VisionUtils()
    
    # For FPS calculation
    prev_time = time.time()
    frame_count = 0
    fps = 0
    
    try:
        while True:
            try:
                frame = frame_queue.get(timeout=1)
                
                # Preprocess the frame
                processed = image_processor.preprocess(frame)
                
                # Detect cones
                detections = color_detector.detect_cones(frame)
                blue_cones = detections['blue_cones']
                orange_cones = detections['orange_cones']
                
                # Draw detections
                frame = vision_utils.draw_cones(frame, blue_cones, (255, 0, 0), 'Blue')
                frame = vision_utils.draw_cones(frame, orange_cones, (0, 165, 255), 'Orange')
                
                # Calculate FPS
                frame_count += 1
                current_time = time.time()
                elapsed = current_time - prev_time
                if elapsed >= 1.0:
                    fps = frame_count / elapsed
                    frame_count = 0
                    prev_time = current_time
                
                # Display FPS
                frame = vision_utils.display_fps(frame, fps)
                
                # Show the frame
                cv2.imshow('Main - Cone Detection', frame)
                
                # Implement further decision making and control logic here
                
                # Exit on 'q' key press
                if cv2.waitKey(1) & 0xFF == ord('q'):
                    logger.info("Exiting main application.")
                    break
            except Empty:
                logger.warning("No frame available in the queue.")
                continue
    except KeyboardInterrupt:
        logger.info("Interrupted by user.")
    finally:
        # Stop frame capturing
        frame_capture.stop()
        cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
