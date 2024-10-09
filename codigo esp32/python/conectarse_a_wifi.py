import network
import time

# Configura el modo de la interfaz de red (STA: Station)
station = network.WLAN(network.STA_IF)
station.active(True)

# Conéctate a la red Wi-Fi (cambia 'SSID' y 'password' por los de tu red)
station.connect("ssid", "password")

# Espera a que se conecte
while not station.isconnected():
    print("Conectando a la red Wi-Fi...")
    time.sleep(1)

# Una vez conectado
print("Conexión establecida!")
print("Dirección IP:", station.ifconfig()[0])  # Muestra la dirección IP obtenida
