# Estación Ambiental ESP32

Repositorio con código fuente y guías para construir una **estación de monitoreo ambiental basada en ESP32**. Incluye ejemplos progresivos para el sensor de calidad del aire MQ-135, el sensor de temperatura y presión BMP280, y una versión completa con RTC DS1307, tarjeta MicroSD y telemetría mediante AskSensors.

## Contenido del repositorio

- [`mq135_inicial/`](mq135_inicial/): ejemplo inicial para probar el MQ-135.
- [`mq135_aire_limpio/`](mq135_aire_limpio/): ejemplo para obtener la referencia del sensor en aire limpio.
- [`prueba_escaner_i2c/`](prueba_escaner_i2c/): escáner para identificar dispositivos conectados al bus I2C.
- [`estacion_mq135_bme280/`](estacion_mq135_bme280/): monitoreo de gases con MQ-135 y de temperatura, presión y altitud con BMP280; presenta las lecturas por el monitor serial.
- [`estacion_mq135_bme280_rtc_microsd/`](estacion_mq135_bme280_rtc_microsd/): versión con RTC DS1307, registro CSV en MicroSD y envío de temperatura, presión, CO₂ y CO a AskSensors por WiFi.

Cada carpeta incluye un sketch `.ino` y una guía propia con los detalles de esa implementación.

## Requisitos

- Arduino IDE y soporte para placas ESP32 instalado desde el Gestor de Tarjetas.
- Driver USB-serial apropiado para la placa (por ejemplo, CP210x si corresponde).
- Librerías de Arduino:
  - `MQUnifiedsensor`
  - `Adafruit BMP280 Library`
  - `Adafruit Unified Sensor`
  - `RTClib` para la versión con RTC
  - Las bibliotecas estándar `Wire`, `SPI`, `SD`, `WiFi` y `HTTPClient` para las funciones correspondientes.

## Conexiones principales

| Componente | Señal | ESP32 | Uso |
| --- | --- | --- | --- |
| MQ-135 | A0 / salida analógica | GPIO34 | Lectura analógica (ADC) |
| BMP280 y RTC DS1307 | SDA | GPIO21 | Bus I2C compartido |
| BMP280 y RTC DS1307 | SCL | GPIO22 | Bus I2C compartido |
| Lector MicroSD | CS | GPIO5 | Selección SPI (`chipSelect`) |
| Lector MicroSD | MOSI / MISO / SCK | Pines SPI estándar | Bus SPI |

> **Importante:** verifica la tensión de salida del módulo MQ-135 antes de conectarlo al ADC del ESP32, que no es tolerante a 5 V. Usa un divisor de tensión o acondicionamiento de nivel adecuado para mantener la señal dentro del rango seguro; una resistencia en serie por sí sola no garantiza la reducción de tensión. Consulta también las guías específicas de cada montaje y la alimentación requerida por los módulos.

## Empezar

1. Clona o descarga este repositorio.
2. Instala Arduino IDE, el soporte para ESP32 y las bibliotecas necesarias.
3. Abre en Arduino IDE el `.ino` de la carpeta que quieras probar.
4. Selecciona **ESP32 Dev Module** (o la placa equivalente) y el puerto correcto.
5. Para el ejemplo básico, abre `estacion_mq135_bme280/estacion_mq135_bme280.ino`. Para el montaje completo, abre `estacion_mq135_bme280_rtc_microsd/estacion_mq135_bme280_rtc_microsd.ino`.
6. En la versión con conectividad, configura `ssid`, `password` y `apiKeyIn` en el sketch antes de cargarlo. No publiques credenciales reales.
7. Abre el monitor serie a **115200 baudios** para revisar el estado y las lecturas.

## Funciones de la estación completa

La versión `estacion_mq135_bme280_rtc_microsd` registra las mediciones en `/datos.csv`, usa un RTC DS1307 para la fecha y hora, y envía datos a AskSensors cada 25 segundos. El sketch usa GPIO21/GPIO22 para I2C y GPIO5 como CS de la MicroSD. El MQ-135 se conecta a GPIO34.

Las lecturas de gases del MQ-135 son estimaciones derivadas de la calibración y de las curvas de respuesta configuradas en el código; no sustituyen un instrumento de medición certificado. Los valores de calibración `R0` son específicos del montaje y deben verificarse para cada sensor.

## Licencia

Este proyecto se distribuye bajo la licencia MIT. Consulta [`LICENSE`](LICENSE) para más información.
