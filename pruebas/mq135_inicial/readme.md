# Lectura Analógica Directa del Sensor MQ-135 en ESP32

Este script en C++ para Arduino IDE realiza la lectura analógica directa (ADC) del sensor de calidad del aire MQ-135 conectado al pin **GPIO34** del ESP32, convirtiendo la señal obtenida en un valor de voltaje equivalente.

---

## 📌 ¿Cómo funciona?

1. **Configuración de Entrada**: Asigna el pin `GPIO34` como entrada analógica y establece la comunicación por puerto serie a `115200 baudios`.
2. **Lectura del ADC**: Captura el valor analógico crudo del sensor mediante `analogRead()`. En el ESP32, este valor varía en una resolución de 12 bits (de `0` a `4095`).
3. **Conversión a Voltaje**: Calcula el voltaje aproximado de la señal aplicando la relación basada en el voltaje de referencia del ADC ($3.3\,\text{V}$):
   $$\text{Voltaje} = \left(\frac{\text{Lectura RAW}}{4095.0}\right) \times 3.3$$
4. **Impresión de Datos**: Muestra en el Monitor Serie la lectura cruda y el voltaje calculado en intervalos de 1 segundo.

---

## 🛠️ Pines y Conexión

| Componente | Pin Sensor | Pin ESP32 | Descripción |
| :--- | :--- | :--- | :--- |
| **MQ-135** | **S (Salida A0)** | **GPIO 34** | Entrada de señal analógica (ADC1_CH6) |
| **MQ-135** | **VCC** | **V** | Alimentación |
| **MQ-135** | **GND** | **G** | Tierra |

> ⚠️ **Nota:** El sensor MQ-135 requiere un tiempo previo de precalentamiento (pre-heating) para estabilizar la resistencia de su elemento calefactor y entregar lecturas precisas.
