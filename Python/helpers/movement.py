from helpers.communication import transmit

def move_stop():
    transmit("| 00 30\n")
    print("STOPPED")
