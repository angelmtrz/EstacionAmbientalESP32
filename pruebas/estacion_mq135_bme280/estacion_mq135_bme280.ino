#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <MQUnifiedsensor.h>

// --- CONFIGURACIÓN MQ135 ---
#define Placa             "ESP32"
#define Voltaje_Resolucion 3.3      // El ADC del ESP32 mide hasta 3.3V
#define Pin_Analogico     34       // El pin 'S' en tu placa de expansión
#define Tipo_Sensor       "MQ-135"
#define Resolucion_ADC    12       // El ESP32 tiene un ADC de 12 bits (0-4095)
#define Ratio_Aire_Limpio 3.6      // Valor estándar para el MQ135 en aire libre
// Remplaza el '4.25' por el número real que te dio el paso anterior
float mi_R0_calibrado = 0.84; 

MQUnifiedsensor MQ135(Placa, Voltaje_Resolucion, Resolucion_ADC, Pin_Analogico, Tipo_Sensor);
// --- CONFIGURACIÓN BME280 ---
Adafruit_BMP280 bmp; // Crea el objeto para controlar el BMP280

void setup() {
  Serial.begin(115200);
  
  MQ135.setRegressionMethod(1); 
  MQ135.init();
  MQ135.setR0(mi_R0_calibrado); 

  // 2. Forzar el inicio del bus I2C en los pines del ESP32 30p
  Serial.println("Iniciando bus I2C (SDA=21, SCL=22)...");
  Wire.begin(21, 22); 

  // 3. Inicializar el BMP280 usando la dirección confirmada 0x76
  Serial.println("Conectando con el sensor de presión/temperatura...");
  if (!bmp.begin(0x76)) {  
    Serial.println("¡Error crítico! El sensor no respondió como BMP280.");
    while (1); 
  }
  
  // Opciones de configuración opcionales para el BMP280 (mejoran estabilidad)
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     
                  Adafruit_BMP280::SAMPLING_X2,     
                  Adafruit_BMP280::SAMPLING_X16,    
                  Adafruit_BMP280::FILTER_X16,      
                  Adafruit_BMP280::STANDBY_MS_500); 

  Serial.println("¡Perfecto! Ambos sensores listos y operando.");

}

void loop() {
  MQ135.update(); // Actualiza la lectura analógica actual del ESP32

  // --- OBTENER CO2 (Dióxido de Carbono) ---
  MQ135.setA(110.47); MQ135.setB(-2.862); 
  float CO2 = MQ135.readSensor();

  // --- OBTENER MONÓXIDO DE CARBONO (CO) ---
  MQ135.setA(605.18); MQ135.setB(-3.937);  
  float CO = MQ135.readSensor();

  // --- OBTENER AMONÍACO (NH4) ---
  MQ135.setA(102.2 ); MQ135.setB(-2.473); 
  float NH4 = MQ135.readSensor();

  // --- OBTENER ALCOHOL ---
  MQ135.setA(77.255); MQ135.setB(-3.18); 
  float Alcohol = MQ135.readSensor();

  // --- LECTURAS DIGITALES BMP280 ---
  float temperatura = bmp.readTemperature(); 
  float presion     = bmp.readPressure() / 100.0F; // Convertido a hPa
  float altitud     = bmp.readAltitude(1013.25);   // Altitud estimada (Presión estándar a nivel del mar)

  // --- IMPRESIÓN DE RESULTADOS ---
  Serial.println("\n========= ESTACIÓN AMBIENTAL =========");
   // Datos del BMP280
  Serial.print("Temperatura : "); Serial.print(temperatura); Serial.println(" °C");
  Serial.print("Presión Atm.: "); Serial.print(presion);     Serial.println(" hPa");
  Serial.print("Altitud est.: "); Serial.print(altitud);     Serial.println(" m");
  Serial.println(" ");
  // Datos MQ135
  Serial.print("CO2    : "); Serial.print(CO2 + 400); Serial.println(" PPM (Sumando 400ppm base atmosférica)"); 
  Serial.print("CO     : "); Serial.print(CO);        Serial.println(" PPM");
  Serial.print("Amoníaco: "); Serial.print(NH4);       Serial.println(" PPM");
  Serial.print("Alcohol : "); Serial.print(Alcohol);   Serial.println(" PPM");
  Serial.println("====================================");
  
  delay(3000); // Muestreo cada 3 segundos

}
