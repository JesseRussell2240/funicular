import serial
import time

# Configure the serial connection
SERIAL_PORT = "/dev/serial0"  # Serial port for UART (change if needed)
BAUD_RATE = 9600              # Common baud rate for UART

# Initialize the serial connection
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)

# Transmit function
def transmit(data):
    # Send data over UART
    ser.write(data.encode())  # Convert string to bytes
    print("Transmitting:", data)

# Receive function
def receive():
    received_data = ser.read(ser.in_waiting or 1).decode('utf-8')  # Read available data
    return received_data if received_data else None

# Main function
if __name__ == "__main__":    
    try:
        while True:
            # Input message to transmit
            message = input("Enter message to transmit (type 'q' to quit): ")
            
            if message.lower() == 'q':
                print("Quitting...")
                break
            
            # Transmit the typed message
            transmit(message + '\n')  # Ensure message ends with newline
            
            time.sleep(1)  # Small delay before receiving response
            
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
