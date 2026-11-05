# EstacionAmbientalESP32

Código fuente y guía de ensamblaje para el proyecto de la **Estación de Monitoreo Ambiental** basada en ESP32.

---

## 🛠️ Requisitos Previos

Antes de comenzar la programación, asegúrate de contar con los siguientes drivers y software:

1. **Driver del Chip Serial:**  
   Descarga e instala el driver [CP210x Universal Windows Driver](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers) para que Windows 11 reconozca correctamente la placa ESP32 Devkit.
2. **Entorno de Desarrollo:**  
   Instala y configura [Arduino IDE](https://www.arduino.cc/en/software).
3. **Soporte para ESP32:**  
   Instala el paquete de tarjetas ESP32 desde el Gestor de Tarjetas (*Board Manager*) en Arduino IDE.
4. **Librerías Necesarias:**  
   Desde el Gestor de Bibliotecas de Arduino IDE, busca e instala:
   - `MQUnifiedsensor`

---

## 🔌 Conexión de Hardware

Sigue los siguientes pasos para realizar las conexiones de la estación:

1. **Placa Base:** Conecta la tarjeta ESP32 a la placa de expansión de 30 pines.
2. **Sensor de Calidad de Aire (MQ-135):**
   Utiliza cables jumper para conectar el sensor a las columnas **GVS** correspondientes al **GPIO34 (D34)** de la placa de expansión:
   - **GND (Sensor):** Conectar a la columna **G** del GPIO34.
   - **VCC (Sensor):** Conectar a la columna **V**.
   - **A0 (Salida Análoga del Sensor):** Conectar a la columna **S** del GPIO34.

> [!CAUTION]
> **Protección del ESP32:**  
> Es estrictamente necesario colocar una **resistencia de 10KΩ (1/2W) en serie** entre el pin **A0** del sensor y la columna **S** del GPIO34 para proteger los pines de entrada del ESP32 contra sobrevoltajes.

---

## 💻 Programación y Carga del Código

1. Conecta el módulo mediante un cable USB Tipo C al puerto correspondiente en la placa de expansión.
2. Abre el proyecto en Arduino IDE y selecciona la placa **ESP32 Dev Module** y el puerto COM asignado.
3. Carga el código de ejemplo a la placa.

```cpp
// Ejemplo base para prueba de sensor MQ-135
#include <MQUnifiedsensor.h>

#define Board ("ESP32")
#define Pin (34) // GPIO34
#define Type ("MQ-135")
#define Voltage_Resolution (3.3)
#define ADC_Bit_Resolution (12)
#define RatioMQ135CleanAir (3.6)

MQUnifiedsensor MQ135(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin, Type);

void setup() {
  Serial.begin(115200);
  MQ135.setRegressionMethod(1); // _PPM = a*ratio^b
  MQ135.init();
  MQ135.setRL(10); // Valor de Resistencia de Carga en KΩ
}

void loop() {
  MQ135.update();
  float ppm = MQ135.readSensor();
  Serial.print("Lectura de CO2/Gas: ");
  Serial.print(ppm);
  Serial.println(" ppm");
  delay(2000);
}
```

---

## 📄 Licencia

Este proyecto está bajo la licencia MIT. Consulta el archivo `LICENSE` para más información.
