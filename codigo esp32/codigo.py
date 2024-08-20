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




from machine import Pin
import time

# Configura el pin del receptor IR
recv_pin = Pin(15, Pin.IN)

# Variables para almacenar el estado del receptor
last_state = 0
current_state = 0

# Buffer para almacenar el código IR
ir_code = []

def read_ir():
    global last_state, current_state, ir_code
    
    current_state = recv_pin.value()
    if current_state != last_state:
        if current_state == 1:
            # Aquí puedes agregar lógica para almacenar y procesar el código IR
            ir_code.append(current_state)
        last_state = current_state

def print_ir_code():
    if ir_code:
        # Convierte el buffer de código IR a hexadecimal y lo imprime
        hex_code = ''.join(f'{x:02X}' for x in ir_code)
        print("Código IR en hexadecimal:", hex_code)
        ir_code.clear()

def main():
    while True:
        read_ir()
        print_ir_code()
        time.sleep(0.1)

if __name__ == '__main__':
    main()
