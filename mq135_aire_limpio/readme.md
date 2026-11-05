# Calibración del Sensor MQ-135 en Aire Limpio (ESP32)

Este script en C++ para Arduino IDE utiliza la librería **MQUnifiedsensor** para realizar el proceso de calibración inicial del sensor **MQ-135** en un entorno de aire limpio, calculando el valor base de resistencia de referencia ($R_0$) necesario para realizar mediciones precisas de gases.

---

## 📌 ¿Cómo funciona?

1. **Definición de Parámetros**: Define la plataforma (`ESP32`), la resolución de voltaje ($3.3\,\text{V}$), el pin de entrada analógica (`GPIO34`), la resolución del ADC ($12\,\text{bits}$) y la relación de resistencia del sensor en aire limpio ($\text{Ratio} = 3.6$)[cite: 3].
2. **Inicialización y Método**: Configura el método de regresión exponencial mediante `setRegressionMethod(1)` e inicializa el sensor[cite: 3].
3. **Muestreo de Calibración**: En la función `setup()`, toma 10 lecturas consecutivas del sensor expuesto a aire limpio para calcular el promedio de $R_0$[cite: 3].
4. **Obtención del $R_0$ Base**: Muestra en el Monitor Serie el valor final de $R_0$ ($R_0 = \frac{\sum \text{calibraciones}}{10}$)[cite: 3]. Este valor debe anotarse e incluirse en los scripts de lectura de concentración de gases.

---

## 🛠️ Parámetros de Configuración

| Parámetro | Valor | Descripción |
| :--- | :--- | :--- |
| **Placa** | `"ESP32"` | Microcontrolador objetivo[cite: 3] |
| **Voltaje_Resolucion** | `3.3` | Tensión de referencia del ADC ($3.3\,\text{V}$)[cite: 3] |
| **Pin_Analogico** | `34` | GPIO asignado a la señal del sensor[cite: 3] |
| **Resolucion_ADC** | `12` | Resolución de 12 bits ($0 - 4095$)[cite: 3] |
| **Ratio_Aire_Limpio** | `3.6` | Coeficiente estándar $R_S/R_0$ para el MQ-135 en aire limpio[cite: 3] |

---

## 📚 Requisitos de Librerías

* **MQUnifiedsensor**: Instalar desde el Gestor de Librerías de Arduino IDE.
