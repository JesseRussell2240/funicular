import serial
import time

# Configure the serial connection
SERIAL_PORT = "/dev/serial0"  # Serial port for UART (change if needed)
BAUD_RATE = 9600              # Common baud rate for UART

# Initialize the serial connection
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)

# Transmit function (sending string)
def transmit(data):
    ser.write(data.encode())  # Send the string as bytes
    print("Transmitting:", data)

# Receive function (receiving string)
def receive():
    data = ser.readline().decode('utf-8').strip()  # Read until newline
    return data if data else None

# Main function
if __name__ == "__main__":
    try:
        while True:
            # Transmit data
            transmit("Hello STM32")  # Transmitting a string
            
            # Receive data
            received_data = receive()
            if received_data:
                print("Received:", received_data)
            
            time.sleep(1)  # Delay before next transmission

    except KeyboardInterrupt:
        print("Exiting...")
    finally:
        # Close the serial connection
        ser.close()
