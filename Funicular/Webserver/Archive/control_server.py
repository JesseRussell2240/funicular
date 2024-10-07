import socket

def handle_client(client_socket):
    """Process incoming control commands."""
    command = client_socket.recv(1024).decode()
    print(f"Received command: {command}")

    # Handle the device movement commands here
    if command == 'forward':
        print("Moving forward")
    elif command == 'back':
        print("Moving back")
    elif command == 'left':
        print("Turning left")
    elif command == 'right':
        print("Turning right")
    else:
        print("Unknown command")

    client_socket.close()

def main():
    """Control server to handle movement commands."""
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind(('0.0.0.0', 5001))  # Listen on port 5001
    server.listen(5)
    print("Control server listening on port 5001")

    while True:
        client_socket, addr = server.accept()
        print(f"Connection from {addr}")
        handle_client(client_socket)

if __name__ == "__main__":
    main()
