from machine import Pin
import time

# Configura el pin del receptor IR
recv_pin = Pin(15, Pin.IN)

# Variables para almacenar el estado del receptor
last_state = 0
current_state = 0
last_detect_time = 0
debounce_time = 0.1  # 100 ms

def read_ir():
    global last_state, current_state, last_detect_time
    current_state = recv_pin.value()
    current_time = time.time()

    if current_state != last_state and current_state == 1:
        # Verifica si ha pasado suficiente tiempo desde la última detección
        if current_time - last_detect_time > debounce_time:
            print("Señal IR detectada")
            last_detect_time = current_time

    last_state = current_state

def main():
    while True:
        read_ir()
        time.sleep(0.01)

if __name__ == '__main__':
    main()