from machine import Pin
import time


# Configura el pin del receptor IR
recv_pin = Pin(15, Pin.IN)


# Variables para almacenar el estado del receptor
last_state = 0
current_state = 0


def read_ir():
    global last_state, current_state
    current_state = recv_pin.value()
    if current_state != last_state:
        if current_state == 1:
            print("Señal IR detectada")
        last_state = current_state


def main():
    while True:
        read_ir()
        time.sleep(0.01)


if __name__ == '__main__':
    main()
