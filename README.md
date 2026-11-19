# Estación Ambiental ESP32

Proyecto de monitoreo ambiental basado en **ESP32** y desarrollado para la FISI-UNAP. El repositorio contiene el código final de medición y envío de datos, además de ejemplos y pruebas utilizadas durante el desarrollo.

## Conexiones físicas de los sensores

Consulta esta [lista de reproducción de YouTube](https://youtube.com/playlist?list=PLFOCXO8_Xn3w&si=ILuiqXIwQ4eIjgxy) con 3 videos que explican las conexiones físicas de los sensores a la placa ESP32 DevKit V1.

## Código principal: `Codigo_Final`

La implementación que debe usarse para el funcionamiento final se encuentra en [`Codigo_Final/`](Codigo_Final/). Contiene dos programas:

### [`Estacion_Ambiental_Final`](Codigo_Final/Estacion_Ambiental_Final/)

Sketch principal de la estación ambiental. Este programa:

- Lee los sensores **MQ-7**, **MQ-135** y **MiCS-2714** mediante ADC.
- Estima concentraciones de **CO**, **CO2**, **NH3** y **NO2** en ppm.
- Utiliza el divisor de tensión 10 kΩ/10 kΩ para adaptar las señales de los sensores al ESP32.
- Conecta el ESP32 a una red Wi-Fi.
- Sincroniza la hora con un servidor NTP usando la zona horaria de Perú (UTC-5).
- Envía una lectura cada 60 segundos mediante `HTTP POST` a la API de la Estación Ambiental FISI-UNAP.
- Registra cada muestra en formato JSON plano con los campos `medido_en`, `co2_ppm`, `co_ppm`, `nh4_ppm` y `no2_ppm`.

Archivo para cargar en Arduino IDE:

[`Codigo_Final/Estacion_Ambiental_Final/Estacion_Ambiental_Final.ino`](Codigo_Final/Estacion_Ambiental_Final/Estacion_Ambiental_Final.ino)

### [`Calibracion_Sensores_MQ`](Codigo_Final/Calibracion_Sensores_MQ/)

Programa auxiliar para obtener los valores de resistencia base (**R0**) de los sensores MQ-7 y MQ-135 en aire limpio.

- Debe ejecutarse en aire limpio, sin humo ni gases.
- Promedia 100 muestras por sensor.
- Calcula e imprime por el puerto serie los valores `R0_MQ7` y `R0_MQ135`.
- Estos valores deben reemplazarse en las constantes de calibración del sketch final.

Archivo:

[`Codigo_Final/Calibracion_Sensores_MQ/Calibracion_Sensores_MQ.ino`](Codigo_Final/Calibracion_Sensores_MQ/Calibracion_Sensores_MQ.ino)

## Conexiones del montaje final

| Componente | Señal | Pin ESP32 | Uso |
| --- | --- | --- | --- |
| MQ-7 | A0, salida analógica | GPIO34 | Monóxido de carbono (CO) |
| MQ-135 | A0, salida analógica | GPIO35 | Calidad del aire, CO2 y NH3 |
| MiCS-2714 | Salida analógica | GPIO32 | Dióxido de nitrógeno (NO2) |

Los tres sensores usan únicamente entradas del canal ADC1 del ESP32. Para la alimentación y señal de los módulos MQ se recomienda el divisor de tensión indicado en el código.

> **Importante:** verifica la tensión de salida de cada módulo antes de conectarlo al ADC del ESP32. El ADC no debe recibir niveles superiores a 3.3 V.

## Configuración requerida

1. Instala Arduino IDE y el soporte para placas ESP32 desde el Gestor de Tarjetas.
2. Instala o verifica las bibliotecas incluidas por el entorno ESP32:
   - `WiFi`
   - `HTTPClient`
   - `time`
   - `math`
3. Abre `Codigo_Final/Estacion_Ambiental_Final/Estacion_Ambiental_Final.ino`.
4. Sustituye las credenciales de ejemplo por las de tu propia red Wi-Fi.
5. Configura `estacionSlug` y `apiToken` con los datos válidos de tu instalación.
6. Ejecuta primero `Codigo_Final/Calibracion_Sensores_MQ/Calibracion_Sensores_MQ.ino` en aire limpio y espera al menos 3 a 5 minutos de estabilización.
7. Copia los valores obtenidos de `R0_MQ7` y `R0_MQ135` en las constantes del sketch principal.
8. Selecciona la placa **ESP32 Dev Module** o el modelo equivalente, configura el puerto y carga el código final.
9. Abre el monitor serie a **115200 baudios**.

### Seguridad de credenciales

No publiques contraseñas, claves API ni tokens reales. Antes de compartir o subir cambios, reemplaza los valores sensibles del sketch por credenciales propias no comprometidas. Si una credencial ya fue publicada en el historial de Git, revócala y reemplázala.

## Uso de la estación final

Después de cargar el sketch principal:

1. El ESP32 se conecta a la red Wi-Fi configurada.
2. Obtiene fecha y hora mediante NTP con desfase UTC-5.
3. Lee los sensores una vez por minuto.
4. Calcula las concentraciones y construye el paquete JSON.
5. Envía el paquete a la API mediante HTTP POST con autorización Bearer.

Las concentraciones calculadas a partir de los sensores MQ son estimaciones que dependen de la calibración, el ambiente y las curvas configuradas. No sustituyen mediciones realizadas con instrumentos certificados.

## Carpeta `pruebas`

La carpeta [`pruebas/`](pruebas/) contiene ejemplos y versiones anteriores utilizadas para validar componentes por separado:

- Lectura directa del **MQ-135**.
- Obtención de referencia del sensor en aire limpio.
- Escáner del bus I2C.
- Integración gradual del MQ-135 y el BMP280.
- Prueba anterior con MQ-135, BMP280, RTC DS1307, MicroSD y AskSensors.

Estos programas son material de referencia y desarrollo; **no son necesarios para el funcionamiento final del proyecto**. Para montar y operar la versión definitiva, usa únicamente los archivos contenidos en [`Codigo_Final/`](Codigo_Final/).

## Licencia

Este proyecto se distribuye bajo la licencia MIT. Consulta [`LICENSE`](LICENSE) para más información.
