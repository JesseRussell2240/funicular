import logging
import cv2
import threading
import time
from queue import Queue
import platform

# Conditional PiCamera import for Raspberry Pi
if platform.system() == "Linux":
    from Camera.camera_setup import CameraSetup


class FrameCapture:
    def __init__(self, camera_setup, frame_queue, queue_size=10):
        """
        Initialize the frame capture process.

        Args:
            camera_setup (CameraSetup): An instance of the CameraSetup class (for Raspberry Pi).
            frame_queue (Queue): A queue to store captured frames.
            queue_size (int): Maximum number of frames to store in the queue.
        """
        self.camera_setup = camera_setup
        self.frame_queue = frame_queue
        self.queue_size = queue_size
        self.logger = logging.getLogger('FrameCapture')
        self.logger.setLevel(logging.DEBUG)
        ch = logging.StreamHandler()
        ch.setLevel(logging.DEBUG)
        formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
        ch.setFormatter(formatter)
        self.logger.addHandler(ch)

        self.thread = threading.Thread(target=self.capture_frames, daemon=True)
        self.stop_event = threading.Event()

    def start(self):
        """Start the frame capturing thread."""
        self.logger.info("Starting frame capture thread.")
        self.thread.start()

    def stop(self):
        """Stop the frame capturing thread."""
        self.logger.info("Stopping frame capture thread.")
        self.stop_event.set()
        self.thread.join()

    def capture_frames(self):
        """Capture frames from either PiCamera or webcam based on the platform."""

        # Check platform to determine camera type
        if platform.system() == "Linux":
            # Use PiCamera on Raspberry Pi
            camera = self.camera_setup.get_camera()
            stream = camera.capture_continuous(
                format='bgr', use_video_port=True
            )  # Modify as necessary for continuous capture on PiCamera

            for frame in stream:
                # Handle frame resizing and queue insertion
                resized_frame = cv2.resize(frame, self.camera_setup.resolution)
                self._add_frame_to_queue(resized_frame)
                if self.stop_event.is_set():
                    break
            camera.close()

        else:
            # Use OpenCV VideoCapture on Windows or non-Raspberry Pi platforms
            stream = cv2.VideoCapture(0)

            if not stream.isOpened():
                self.logger.error("Failed to open video stream.")
                return

            while not self.stop_event.is_set():
                ret, frame = stream.read()
                if not ret:
                    self.logger.warning("Failed to read frame from camera.")
                    continue

                # Resize frame if necessary
                frame = cv2.resize(frame, self.camera_setup.resolution)

                # Add frame to queue
                self._add_frame_to_queue(frame)

                # Optional: Add a small delay to reduce CPU usage
                time.sleep(0.01)

            stream.release()

        self.logger.info("Frame capture thread terminated.")

    def _add_frame_to_queue(self, frame):
        """Add a frame to the queue if there's space."""
        try:
            self.frame_queue.put_nowait(frame)
            self.logger.debug("Frame captured and added to queue.")
        except:
            self.logger.warning("Frame queue is full. Dropping frame.")
