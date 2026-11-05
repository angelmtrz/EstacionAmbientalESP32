# Lectura Integrada de Calidad del Aire y Clima (MQ-135 + BMP280)

Este script en C++ para Arduino IDE integra las lecturas del sensor analógico de calidad del aire **MQ-135** y el sensor ambiental digital **BMP280** (comunicación I2C) utilizando la placa ESP32[cite: 4]. Permite monitorear en tiempo real la concentración de varios gases (CO₂, CO, NH₄, Alcohol), así como la temperatura, presión atmosférica y altitud aproximada[cite: 4].

---

## 📌 ¿Cómo funciona?

1. **Configuración e Inicialización**:
   - Inicializa el sensor MQ-135 cargando el valor de resistencia base calibrado previo ($R_0 = 0.84$)[cite: 4].
   - Configura el bus I2C en los pines `SDA` (GPIO21) y `SCL` (GPIO22) e inicializa el BMP280 en la dirección hexadecimal `0x76`[cite: 4].
   - Ajusta los parámetros de sobremuestreo y filtrado del BMP280 para mayor estabilidad en las lecturas[cite: 4].
2. **Cálculo de Gases (MQ-135)**:
   - Aplica los coeficientes de regresión exponenciales específicos ($a$ y $b$) para estimar la concentración en PPM (Partes Por Millón) de $\text{CO}_2$, $\text{CO}$, $\text{NH}_4$ y Alcohol[cite: 4].
   - Añade el valor base atmosférico ($\approx 400\,\text{PPM}$) a la lectura final de $\text{CO}_2$[cite: 4].
3. **Lectura Climatológica (BMP280)**:
   - Obtiene la temperatura en grados Celsius (°C), la presión atmosférica en hectopascales (hPa) y calcula la altitud estimada sobre el nivel del mar[cite: 4].
4. **Despliegue de Datos**: Muestra los resultados formateados en el Monitor Serie cada 3 segundos[cite: 4].

---

## 🛠️ Conexión de Hardware

| Componente | Pin Sensor | Pin ESP32 | Protocolo / Función |
| :--- | :--- | :--- | :--- |
| **MQ-135** | **S (Salida A0)** | **GPIO 34** | Entrada analógica (ADC1_CH6)[cite: 4] |
| **BMP280** | **SDA** | **GPIO 21** | Datos I2C[cite: 4] |
| **BMP280** | **SCL** | **GPIO 22** | Reloj I2C[cite: 4] |

---

## 📊 Coeficientes de Regresión Utilizados (MQ-135)

| Gas Calculado | Coeficiente $a$ | Coeficiente $b$ |
| :--- | :--- | :--- |
| **Dióxido de Carbono ($\text{CO}_2$)** | `110.47` | `-2.862` |
| **Monóxido de Carbono ($\text{CO}$)** | `605.18` | `-3.937` |
| **Amoníaco ($\text{NH}_4$)** | `102.20` | `-2.473` |
| **Alcohol** | `77.255
