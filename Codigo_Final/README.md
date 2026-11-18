# Código final de la estación ambiental ESP32

Esta carpeta contiene los dos sketches para preparar y ejecutar la estación ambiental. **El orden de uso es importante:** primero calibra los sensores MQ-7 y MQ-135; después copia los valores obtenidos al sketch principal y ejecuta la estación para enviar las mediciones a la API.

## Contenido

- [`Calibracion_Sensores_MQ/Calibracion_Sensores_MQ.ino`](Calibracion_Sensores_MQ/Calibracion_Sensores_MQ.ino): calcula los valores de resistencia base `R0` para MQ-7 y MQ-135 en aire limpio.
- [`Estacion_Ambiental_Final/Estacion_Ambiental_Final.ino`](Estacion_Ambiental_Final/Estacion_Ambiental_Final.ino): lee los sensores MQ-7, MQ-135 y MiCS-2714, calcula las concentraciones configuradas y envía los datos a la API FISI-UNAP.

## Requisitos y conexiones

- Placa **ESP32 DevKit V1** (o compatible) y Arduino IDE con soporte para ESP32.
- Red Wi-Fi con acceso a Internet para sincronización de hora y envío a la API.
- Sensores MQ-7, MQ-135 y MiCS-2714.
- Divisores de tensión **10 kΩ/10 kΩ** para las señales analógicas de MQ-7 y MQ-135, tal como contempla el código.

| Sensor | Salida analógica | GPIO ESP32 | Uso en el programa |
| --- | --- | --- | --- |
| MQ-7 | A0 | GPIO34 | Estimación de CO |
| MQ-135 | A0 | GPIO35 | Estimación de CO2 y NH3 |
| MiCS-2714 | Salida analógica | GPIO32 | Estimación de NO2 |

Los pines utilizados son canales ADC1. **No conectes al ESP32 una señal que supere los 3.3 V.** Comprueba las salidas y el divisor de tensión antes de alimentar o conectar el circuito. Sigue también las recomendaciones del fabricante de cada sensor, especialmente las relativas a alimentación y calentamiento.

## Paso 1: calibrar MQ-7 y MQ-135

1. Conecta el MQ-7 a GPIO34 y el MQ-135 a GPIO35, usando los divisores de tensión indicados.
2. Coloca los sensores en un ambiente de aire limpio, al aire libre y lejos de humo, gases de combustión o gas de cocina.
3. Deja que los sensores se calienten y estabilicen durante **al menos 3 a 5 minutos** antes de usar los resultados. Los sensores nuevos pueden requerir un acondicionamiento inicial adicional según su fabricante.
4. Abre `Calibracion_Sensores_MQ/Calibracion_Sensores_MQ.ino` en Arduino IDE, selecciona la placa y el puerto, y carga el sketch.
5. Abre el Monitor Serie a **115200 baudios**. El sketch configura el ADC a 12 bits, espera 10 segundos al iniciar y luego promedia 100 lecturas por sensor, separadas por 100 ms.
6. Anota los valores `R0_MQ7` y `R0_MQ135` que imprime. El programa vuelve a realizar el cálculo periódicamente.

El sketch mide el voltaje promedio, estima la resistencia del sensor `Rs` usando `RL_MODULE_KOHM = 10 kΩ` y obtiene `R0` aplicando las relaciones aire limpio configuradas en el código (MQ-7: `Rs/R0 ≈ 27`; MQ-135: `Rs/R0 ≈ 3.6`). Estas relaciones son referencias para la calibración, no una calibración certificada.

## Paso 2: configurar y ejecutar la estación

1. Abre `Estacion_Ambiental_Final/Estacion_Ambiental_Final.ino`.
2. Configura los datos propios de la instalación en las constantes de red y API:
   - `ssid`: nombre de la red Wi-Fi.
   - `password`: contraseña Wi-Fi.
   - `estacionSlug`: identificador (slug) de la estación registrado en la API.
   - `apiToken`: token de autorización Bearer.
   - `serverBaseUrl`: URL base de la API, si corresponde cambiarla.
3. Reemplaza los valores de ejemplo `R0_MQ7` y `R0_MQ135` por los que obtuviste en el paso de calibración. Ambos valores deben expresarse en kΩ, como los imprime el sketch de calibración.
4. Conecta los tres sensores a los pines indicados en la tabla. Carga el sketch principal en el ESP32.
5. Abre el Monitor Serie a **115200 baudios** para revisar la conexión Wi-Fi, la dirección IP y las respuestas de la API.

> No compartas ni subas credenciales reales al repositorio. Usa tus propios valores localmente y evita incluir contraseñas o tokens en commits públicos. Si un token real se publicó anteriormente, revócalo y genera uno nuevo.

## Qué hace el sketch principal

Al arrancar, el programa configura el ADC con resolución de 12 bits, intenta conectarse a Wi-Fi y configura la sincronización de hora mediante `pool.ntp.org`, con zona horaria UTC-5 para Perú. Después realiza un envío inmediatamente y repite el ciclo cada **60 segundos**.

En cada ciclo:

1. Comprueba el estado de la conexión Wi-Fi; si está desconectada, solicita una reconexión y no envía esa lectura.
2. Obtiene la fecha y hora local. Si no está disponible, omite el envío.
3. Lee los voltajes analógicos de los tres sensores. Para MQ-7 y MQ-135 aplica la compensación del divisor 10 kΩ/10 kΩ.
4. Calcula las estimaciones en ppm según las fórmulas incluidas en el sketch:
   - MQ-7: CO (`co_ppm`).
   - MQ-135: CO2 (`co2_ppm`) y NH3 (`nh4_ppm`).
   - MiCS-2714: NO2 (`no2_ppm`).
5. Construye un JSON con la marca de tiempo ISO-8601 y las mediciones.
6. Envía el JSON mediante una petición HTTP `POST` al endpoint formado por `serverBaseUrl + estacionSlug + "/lecturas"`. Incluye `Content-Type: application/json` y la cabecera de autorización Bearer.
7. Imprime en el Monitor Serie el endpoint, el contenido enviado, el código HTTP y la respuesta recibida, o un mensaje de error de comunicación.

El cuerpo enviado tiene esta estructura:

```json
{
  "medido_en": "AAAA-MM-DDTHH:MM:SS-05:00",
  "co2_ppm": 0.0,
  "co_ppm": 0.0,
  "nh4_ppm": 0.0,
  "no2_ppm": 0.0
}
```

La hora se configura para UTC-5 y el formato de fecha incluye el desplazamiento `-05:00`. El código muestra mensajes específicos para respuestas HTTP 200/201 (éxito), 401 (token no válido), 422 (parámetros rechazados) y 429 (límite de solicitudes), además de los errores de transporte.

## Notas sobre las mediciones

Las concentraciones son **estimaciones calculadas por las fórmulas y constantes del sketch**. Dependen de la calibración, del circuito, del calentamiento, de las condiciones ambientales y de la respuesta de cada sensor; no equivalen a mediciones de un instrumento certificado. El MQ-135 tiene sensibilidad cruzada a varios gases, por lo que sus estimaciones de CO2 y NH3 no deben interpretarse como mediciones selectivas certificadas.

El programa de calibración de esta carpeta solo calcula `R0` para MQ-7 y MQ-135. El MiCS-2714 no tiene un procedimiento de calibración separado aquí; la conversión de NO2 está definida directamente en el sketch principal.
