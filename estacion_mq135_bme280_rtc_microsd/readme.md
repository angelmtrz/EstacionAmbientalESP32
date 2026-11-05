# Estación Ambiental Completa (ESP32 + MQ-135 + BMP280 + RTC DS1307 + MicroSD + AskSensors)

Este script en C++ para Arduino IDE implementa un sistema completo de monitoreo ambiental IoT y Datalogger en la placa ESP32[cite: 5]. El código realiza lecturas del sensor de calidad del aire **MQ-135** y del sensor de clima **BMP280**, marca los datos en tiempo real mediante un RTC **DS1307**, los almacena localmente en una tarjeta **MicroSD** (formato CSV) y transmite la información a la plataforma en la nube **AskSensors** vía Wi-Fi[cite: 5].

---

## 📌 Funcionalidades Principales

1. **Gestión de Tiempo Real**: Usa el chip RTC DS1307 para estampar la fecha y hora (`YYYY-MM-DD HH:MM:SS`) en cada registro de datos[cite: 5].
2. **Datalogger Local (MicroSD)**: Genera y actualiza dinámicamente un archivo `/datos.csv` en la memoria MicroSD, creando automáticamente el encabezado si el archivo no existe[cite: 5].
3. **Telemetría IoT (AskSensors)**: Envía peticiones HTTP GET cada 25 segundos para actualizar los módulos del dashboard en la nube (Temperatura, Presión, $\text{CO}_2$ y $\text{CO}$)[cite: 5].
4. **Cálculo de Calidad del Aire y Clima**:
   - Mide concentración en PPM para $\text{CO}_2$, $\text{CO}$, $\text{NH}_4$ y Alcohol aplicando la resistencia base calibrada ($R_0 = 40.53$) y coeficientes de regresión exponencial[cite: 5].
   - Mide temperatura (°C), presión atmosférica (hPa) y calcula la altitud aproximada (m)[cite: 5].

---

## 🛠️ Conexión de Hardware y Pines

| Componente | Pin / Señal Sensor | Pin ESP32 | Protocolo / Función |
| :--- | :--- | :--- | :--- |
| **MQ-135** | **S (Salida A0)** | **GPIO 34** | Entrada analógica (ADC1_CH6)[cite: 5] |
| **BMP280 / RTC** | **SDA** | **GPIO 21** | Bus I2C (Datos compartidos)[cite: 5] |
| **BMP280 / RTC** | **SCL** | **GPIO 22** | Bus I2C (Reloj compartido)[cite: 5] |
| **Lector MicroSD** | **CS** | **GPIO 5** | Seleccionador de chip SPI (`chipSelect`)[cite: 5] |
| **Lector MicroSD** | **MOSI / MISO / SCK** | **Pines SPI estándar** | Bus de datos SPI |

---

## 🌐 Configuración de AskSensors (Módulos HTTP GET)

El sistema envía los datos estructurados en la siguiente llamada a la API[cite: 5]:

$$\text{URL} = \text{\texttt{http://api.asksensors.com/write/\{apiKeyIn\}?module1=\{Temp\}\&module2=\{Presión\}\&module3=\{CO2\}\&module4=\{CO\}}}$$
[cite: 5]

| Módulo AskSensors | Variable Asignada | Unidad |
| :--- | :--- | :--- |
| **module1** | Temperatura | °C[cite: 5] |
| **
