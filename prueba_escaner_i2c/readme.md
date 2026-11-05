# Escáner de Bus I2C para ESP32

Este script en C++ para Arduino IDE permite identificar la dirección hexadecimal de cualquier dispositivo i2C (como sensores BME280, RTC DS3231, pantallas OLED, etc.) conectado a los pines del ESP32.

---

## 📌 ¿Cómo funciona?

1. **Inicialización**: Configura la comunicación I2C en los pines `SDA` (GPIO21) y `SCL` (GPIO22), además del puerto serie a `115200 baudios`.
2. **Barrido de Direcciones**: Recorre en bucle todas las direcciones posibles de 7 bits del protocolo I2C (de la `0x01` a la `0x7E`).
3. **Verificación de Respuesta**: Envía una transmisión de prueba (`Wire.beginTransmission`) a cada dirección. Si el dispositivo responde con un `ACK` (`error == 0`), reporta su dirección en formato hexadecimal por el Monitor Serie.
4. **Ciclo**: Repite el proceso de escaneo automáticamente cada 5 segundos.

---

## 🛠️ Pines Utilizados

| Señal | Pin GPIO ESP32 |
| :--- | :--- |
| **SDA** | GPIO 21 |
| **SCL** | GPIO 22 |
