import network

# Crear un objeto de estación (modo Wi-Fi cliente)
station = network.WLAN(network.STA_IF)
station.active(True)

# Escanear redes disponibles
networks = station.scan()

# Mostrar las redes detectadas
for network in networks:
    ssid, bssid, channel, rssi, authmode, hidden = network
    print("SSID:", ssid.decode('utf-8'), "| Canal:", channel, "| Señal (RSSI):", rssi)
