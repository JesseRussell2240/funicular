import serial

SERIAL_PORT = "/dev/ttyAMA10"
BAUD_RATE = 9600

def transmit(data):
    try:
        with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1) as ser:
            ser.write(data.encode())
            print(f"Transmitted: {data}")
    except Exception as e:
        print(f"Transmission error: {e}")
