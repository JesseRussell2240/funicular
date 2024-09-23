# camera_setup.py

import logging
from picamera import PiCamera
from time import sleep

class CameraSetup:
    def __init__(self, resolution=(640, 480), framerate=30):
        """
        Initialize the camera with specified resolution and frame rate.
        
        Args:
            resolution (tuple): The desired resolution (width, height).
            framerate (int): The desired frame rate.
        """
        self.resolution = resolution
        self.framerate = framerate
        self.camera = None
        self.logger = logging.getLogger('CameraSetup')
        self.logger.setLevel(logging.DEBUG)
        ch = logging.StreamHandler()
        ch.setLevel(logging.DEBUG)
        formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
        ch.setFormatter(formatter)
        self.logger.addHandler(ch)
        
        self.initialize_camera()

    def initialize_camera(self):
        """Configure and initialize the camera hardware."""
        try:
            self.logger.info("Initializing camera...")
            self.camera = PiCamera()
            self.camera.resolution = self.resolution
            self.camera.framerate = self.framerate
            self.camera.iso = 100  # Set ISO for consistent lighting
            sleep(2)  # Allow camera to warm up
            self.logger.info(f"Camera initialized with resolution {self.resolution} and framerate {self.framerate}fps.")
        except Exception as e:
            self.logger.error(f"Failed to initialize camera: {e}")
            raise

    def get_camera(self):
        """Return the initialized camera object."""
        if self.camera is None:
            self.logger.error("Camera not initialized.")
            raise ValueError("Camera not initialized.")
        return self.camera
