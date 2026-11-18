#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "RTClib.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <MQUnifiedsensor.h>
// --- CONFIGURACIÓN WI-FI Y ASKSENSORS ---
const char* ssid = "";     // Coloca el nombre de tu Wi-Fi - SSID
const char* password = "";    // Coloca la contraseña de tu Wi-Fi
const char* apiKeyIn = "";  // Reemplaza con tu Api Key de AskSensors

const char* serverName = "http://api.asksensors.com/write/";

// --- CONFIGURACIÓN MQ135 ---
#define Placa             "ESP32"
#define Voltaje_Resolucion 3.3      // El ADC del ESP32 mide hasta 3.3V
#define Pin_Analogico     34       // El pin 'S' en tu placa de expansión
#define Tipo_Sensor       "MQ-135"
#define Resolucion_ADC    12       // El ESP32 tiene un ADC de 12 bits (0-4095)
#define Ratio_Aire_Limpio 3.6      // Valor estándar para el MQ135 en aire libre

// Remplaza el '4.25' por el número real que te dio el paso anterior
float mi_R0_calibrado = 40.53; 
MQUnifiedsensor MQ135(Placa, Voltaje_Resolucion, Resolucion_ADC, Pin_Analogico, Tipo_Sensor);

// --- CONFIGURACIÓN BME280 y RTC DS1307 ---
Adafruit_BMP280 bmp; // Crea el objeto para controlar el BMP280
RTC_DS1307 rtc;
// Pin CS para el lector Micro SD
const int chipSelect = 5; 

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); // Inicializar bus I2C para BMP280 y RTC
  // 1. Inicializar MQ135
  MQ135.setRegressionMethod(1); 
  MQ135.init();
  MQ135.setR0(mi_R0_calibrado); 
  // 2. Inicializar BMP280
  if (!bmp.begin(0x76)) {  
    Serial.println("¡Error! No se detecta el BMP280.");
    while (1);
  }
  // Opciones de configuración opcionales para el BMP280 (mejoran estabilidad)
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     
                  Adafruit_BMP280::SAMPLING_X2,     
                  Adafruit_BMP280::SAMPLING_X16,    
                  Adafruit_BMP280::FILTER_X16,      
                  Adafruit_BMP280::STANDBY_MS_500); 
  // 3. Inicializar RTC DS1307
  if (!rtc.begin()) {
    Serial.println("¡Error! No se detecta el módulo RTC.");
    while (1);
  }
  // Si el RTC perdió la energía, se actualiza con la hora de compilación de la PC
  if (!rtc.isrunning()) {
    Serial.println("El RTC no está corriendo. Ajustando hora...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  // 4. Inicializar Tarjeta Micro SD
  Serial.println("Inicializando tarjeta Micro SD...");
  if (!SD.begin(chipSelect)) {
    Serial.println("¡Fallo en la tarjeta o no está presente!");
    // No detenemos el código completo aquí para que al menos puedas ver datos por Serial si la SD falla
  } else {
    Serial.println("Tarjeta Micro SD lista.");
    
    // Crear el encabezado del archivo CSV si no existe
    File dataFile = SD.open("/datos.csv", FILE_WRITE);
    if (dataFile) {
      // Si el archivo está vacío, escribimos los títulos de las columnas
      if (dataFile.size() == 0) {
        dataFile.println("Fecha,Hora,Temperatura(C),Presion(hPa),Altitud(m),CO2(PPM),CO(PPM),Amoniaco(PPM),Alcohol(PPM)");

      }
      dataFile.close();
    }
  }
  // 5. Conectar a la red Wi-Fi
  Serial.print("Conectando a Wi-Fi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  // Esperar a que se conecte (puedes omitir el bucle infinito si deseas que funcione offline si no hay Wi-Fi)
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n¡Conectado a Wi-Fi con éxito!");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // --- LEER TIEMPO ---
  DateTime ahora = rtc.now();

  // --- LEER MQ135 ---
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

  // --- CONSTRUIR CADENA DE DATOS (Formato CSV) ---
  String dataString = "";
  dataString += String(ahora.year()) + "-" + String(ahora.month()) + "-" + String(ahora.day()) + ",";
  dataString += String(ahora.hour()) + ":" + String(ahora.minute()) + ":" + String(ahora.second()) + ",";
  dataString += String(temperatura) + "," + String(presion) + "," + String(altitud) + ",";
  dataString += String(CO2) + "," + String(CO) + "," + String(NH4) + "," + String(Alcohol);
  // --- IMPRIMIR EN MONITOR SERIAL ---
  Serial.print("Guardando: ");
  Serial.println(dataString);

  // --- GUARDAR EN TARJETA MICRO SD ---
  File dataFile = SD.open("/datos.csv", FILE_APPEND);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    Serial.println("-> ¡Guardado exitoso!");
  } else {
    Serial.println("-> Error al abrir datos.csv");
  }

  // --- ENVIAR DATOS A ASKSENSORS ---
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    // Estructura de la URL HTTP GET para AskSensors:
    String url = String(serverName) + apiKeyIn 
                                + "?module1=" + String(temperatura) 
                                + "&module2=" + String(presion) 
                                + "&module3=" + String(CO2 + 400) 
                                + "&module4=" + String(CO);
    
    Serial.print("Enviando a AskSensors: ");
    Serial.println(url);
    
    http.begin(url);
    int httpResponseCode = http.GET(); // Realiza la petición GET
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.print("Respuesta del servidor AskSensors: ");
      Serial.println(response); // Si devuelve '1' o la cantidad de módulos actualizados, fue exitoso
    } else {
      Serial.print("Error enviando petición HTTP GET. Código: ");
      Serial.println(httpResponseCode);
    }
    http.end(); // Libera los recursos de la petición
  } else {
    Serial.println("Wi-Fi desconectado. No se pueden enviar datos a la nube.");
  }

  Serial.println("======================================");
  delay(25000); // AskSensors en cuentas gratuitas suele requerir un intervalo mínimo de 10 a 15 segundos entre envíos.

  //delay(5000); // Guardar historial cada 5 segundos

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
  
  //delay(3000); // Muestreo cada 3 segundos

}
