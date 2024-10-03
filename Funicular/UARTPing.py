import RPi.GPIO as GPIO
import time

# Set up GPIO mode
GPIO.setmode(GPIO.BOARD)

# Define GPIO pins for transmission and reception
TX_PIN = 11  # GPIO pin for transmission
RX_PIN = 13  # GPIO pin for reception

# Set up GPIO pins
GPIO.setup(TX_PIN, GPIO.OUT)  # Configure TX pin as output
GPIO.setup(RX_PIN, GPIO.IN)    # Configure RX pin as input

# Transmit function
def transmit(data):
    GPIO.output(TX_PIN, GPIO.HIGH)  # Set TX pin high for transmission
    # Perform transmission here
    print("Transmitting:", data)
    time.sleep(1)  # Simulating transmission delay
    GPIO.output(TX_PIN, GPIO.LOW)   # Set TX pin low after transmission

# Receive function
def receive():
    data = None
    if GPIO.input(RX_PIN) == GPIO.HIGH:  # Check if RX pin is receiving data
        # Perform reception here
        data = "Data received"  # Example received data
    return data

# Main function
if __name__ == "__main__":
    try:
        while True:
            # Transmit data
            transmit("Hello, world!")
            
            # Receive data
            received_data = receive()
            if received_data:
                print("Received:", received_data)
            
            time.sleep(1)  # Delay before next transmission
    except KeyboardInterrupt:
        print("Exiting...")
    finally:
        # Clean up GPIO
        GPIO.cleanup()
