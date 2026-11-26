/*
  Estacion Ambiental FISI-UNAP - ESP32 DevKit V1 (30 Pines, USB-C)
  ---------------------------------------------------------------
  Sensores integrados:
  - MQ-7 (Monoxido de Carbono - CO): Pin GPIO34 (ADC1_CH6) + Divisor de tension 10k/10k
  - MQ-135 (Calidad de Aire / CO2 / NH3): Pin GPIO35 (ADC1_CH7) + Divisor de tension 10k/10k
  - MiCS-2714 MEMS (Dioxido de Nitrogeno - NO2): Pin GPIO32 (ADC1_CH4)
  
  Conexion: HTTP POST (JSON) con token Bearer hacia la API de la Estacion Ambiental FISI-UNAP.
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>
#include <math.h>
// Agregar esta libreria para corregir el problema de Conexión rechazada
#include <WiFiClientSecure.h>

// =======================================================================
// CONFIGURACION DE RED Y API
// =======================================================================
const char* ssid          = "Tu SSID";       // Cambia por el nombre de tu red Wi-Fi
const char* password      = "Tu contraseña de WiFi"; // Cambia por tu contraseña Wi-Fi

const char* estacionSlug  = "El nombre de tu estación (SLUG)";   // Identificador de tu estacion
const char* apiToken      = "Tu API Token";      // Token de autorizacion Bearer FISI-UNAP
const char* serverBaseUrl = "https://fisiunap.edu.pe/estacion-ambiental/api/estaciones/"; // URL base de la API

// Servidor NTP y desfase horario para Perú (UTC-5)
const char* ntpServer     = "pool.ntp.org";
const long  gmtOffset_sec = -5 * 3600; // -5 horas = -18000 segundos
const int   daylightOffset_sec = 0;   // Sin horario de verano

// =======================================================================
// CONSTANTES DE CALIBRACIÓN R0 (REEMPLAZAR CON TUS VALORES OBTENIDOS)
// =======================================================================
// Coloca aquí los valores exactos impresos por CalibracionSensoresMQ.ino:
float R0_MQ7   = 0.45; // Ejemplo: R0 del MQ-7 en kOhm (obtener del Monitor Serie)
float R0_MQ135 = 275; // Ejemplo: R0 del MQ-135 en kOhm (obtener del Monitor Serie)

const float RL_MODULE_KOHM = 10.0; // Resistencia de carga RL integrada en la placa PCB (10kOhm)

// =======================================================================
// ASIGNACIÓN DE PINES ANALÓGICOS (Únicamente canal ADC1)
// =======================================================================
const int PIN_MQ7      = 34; // GPIO34 -> Lectura de CO (co_ppm)
const int PIN_MQ135    = 35; // GPIO35 -> Lectura de CO2 (co2_ppm) y NH3 (nh4_ppm)
const int PIN_MICS2714 = 32; // GPIO32 -> Lectura de NO2 (no2_ppm)

const unsigned long INTERVALO_ENVIO_MS = 60000; // 1 envío por minuto
unsigned long ultimoEnvio = 0;

// =======================================================================
// FUNCIONES AUXILIARES Y MATEMÁTICAS
// =======================================================================

// Lectura de voltaje considerando la atenuación del divisor de tensión (10k / 10k -> x2)
float leerVoltajeSensor(int pin) {
  int lecturaADC = analogRead(pin);
  float voltajeADC = (lecturaADC / 4095.0) * 3.3;
  return voltajeADC * 2.0; // Multiplicamos por 2.0 debido al divisor de tensión
}
//
// Lectura analógica directa de 0 a 3.3V (específica para MiCS-2714 sin divisor) 
float leerVoltajeDirecto(int pin) { 
	int lecturaADC = analogRead(pin); 
	return (lecturaADC / 4095.0) * 3.3; // Sin multiplicar por 2.0 
}
//
// Obtener la resistencia instantánea Rs del sensor
float calcularRs(float voltajeSensor) {
  if (voltajeSensor >= 4.95) voltajeSensor = 4.95;
  if (voltajeSensor <= 0.05) voltajeSensor = 0.05;
  return ((5.0 - voltajeSensor) / voltajeSensor) * RL_MODULE_KOHM;
}

// 1. Cálculo de CO (PPM) para MQ-7 usando curva logarítmica
float calcularCoPpm(float voltaje) {
  float rs = calcularRs(voltaje);
  float ratio = rs / R0_MQ7;
  // Curva de sensibilidad MQ-7 para CO: PPM = 100 * (Rs/R0)^(-1.53)
  float ppm = 100.0 * pow(ratio, -1.53);
  if (ppm < 0.0) ppm = 0.0;
  return ppm;
}

// 2. Cálculo de CO2 (PPM) para MQ-135 usando curva logarítmica
/*
float calcularCo2Ppm(float voltaje) {
  float rs = calcularRs(voltaje);
  float ratio = rs / R0_MQ135;
  // Curva de sensibilidad MQ-135 para CO2: PPM = 116.602 * (Rs/R0)^(-2.769)
  float ppm = 116.602 * pow(ratio, -2.769);
  if (ppm < 400.0) ppm = 400.0; // Línea base natural del aire (~400 ppm)
  return ppm;
}
*/
float calcularCo2Ppm(float voltaje) { 
	float rs = calcularRs(voltaje); 
	float ratio = rs / R0_MQ135; 
	float ppm = 13915.0 * pow(ratio, -2.769); 
	//return (ppm < 0.0) ? 0.0 : ppm; // Muestra fluctuaciones reales alrededor de 400 ppm 
	if (ppm < 400.0) ppm = 400.0; // Línea base natural del aire (~400 ppm)
  	return ppm;
}
// 3. Cálculo de NH3 (PPM) para MQ-135 usando curva logarítmica
float calcularNh4Ppm(float voltaje) {
  float rs = calcularRs(voltaje);
  float ratio = rs / R0_MQ135;
  // Curva de sensibilidad MQ-135 para NH3: PPM = 102.2 * (Rs/R0)^(-2.473)
  float ppm = 102.2 * pow(ratio, -2.473);
  if (ppm < 0.0) ppm = 0.0;
  return ppm;
}

// 4. Cálculo de NO2 (PPM) para MiCS-2714
//float calcularNo2Ppm(float voltaje) {
//  float ppm = (voltaje / 3.3) * 2.0; // Respuesta analógica para MiCS-2714
// if (ppm < 0.0) ppm = 0.0;
//  return ppm;
//}
//
// Función corregida para Dióxido de Nitrógeno (NO2) 
float calcularNo2Ppm(float voltajeDirecto) { 
	float ppm = (voltajeDirecto / 3.3) * 2.0; 
	return (ppm < 0.0) ? 0.0 : ppm; 
}
//
// Obtener fecha y hora en formato ISO-8601 (Perú UTC-5)
String obtenerFechaHoraISO() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("[NTP] Error: No se pudo obtener la hora del servidor.");
    return "";
  }
  char timeBuffer[35];
  strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%dT%H:%M:%S-05:00", &timeinfo);
  return String(timeBuffer);
}

// Envió del paquete mediante HTTP POST
void enviarLecturaAPI() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[ERROR] Wi-Fi desconectado. Reintentando...");
    WiFi.reconnect();
    return;
  }

  String fechaHoraISO = obtenerFechaHoraISO();
  if (fechaHoraISO == "") return;

  // 1. Obtención de voltajes reales compensados
  float vMQ7   = leerVoltajeSensor(PIN_MQ7);
  float vMQ135 = leerVoltajeSensor(PIN_MQ135);
  //float vMiCS  = leerVoltajeSensor(PIN_MICS2714);
  float vMiCS = leerVoltajeDirecto(PIN_MICS2714);  // Lectura sin multiplicar por 2.0 

  // 2. Cálculo de concentraciones PPM calibradas
  float co_ppm  = calcularCoPpm(vMQ7);
  float co2_ppm = calcularCo2Ppm(vMQ135);
  float nh4_ppm = calcularNh4Ppm(vMQ135);
  //float no2_ppm = calcularNo2Ppm(vMiCS);
  float no2_ppm = calcularNo2Ppm(vMiCS);

  // 3. Estructuración del JSON plano requerido por la API FISI-UNAP
  String jsonPayload = "{";
  jsonPayload += "\"medido_en\":\"" + fechaHoraISO + "\",";
  jsonPayload += "\"co2_ppm\":" + String(co2_ppm, 2) + ",";
  jsonPayload += "\"co_ppm\":" + String(co_ppm, 2) + ",";
  jsonPayload += "\"nh4_ppm\":" + String(nh4_ppm, 2) + ",";
  jsonPayload += "\"no2_ppm\":" + String(no2_ppm, 4);
  jsonPayload += "}";
	// ****
  // Agregar este codigo para evitar la revision del certificado de seguridad
  // y el rechazo de la conexión
  // - Crear el cliente seguro
  WiFiClientSecure client;
  // - Configurar setInsecure() ANTES de http.begin()
  client.setInsecure();
  client.setHandshakeTimeout(30); // Evita cortes si el servidor tarda en responder el saludo TLS
  // ****

  // 4. Inicialización cliente HTTP
  HTTPClient http;
  String endpointUrl = String(serverBaseUrl) + estacionSlug + "/lecturas";
  //http.begin(endpointUrl);
// ****
  if (http.begin(client, endpointUrl)){
      http.addHeader("Content-Type", "application/json");
      http.addHeader("Authorization", "Bearer " + String(apiToken));

      Serial.println("\n--------------------------------------------------");
      Serial.println("Enviando telemetría calibrada a la API FISI-UNAP:");
      Serial.println("Endpoint: " + endpointUrl);
      Serial.println("Payload:  " + jsonPayload);

      int httpCode = http.POST(jsonPayload);

      if (httpCode > 0) {
        String response = http.getString();
        Serial.printf("Respuesta HTTP [%d]: %s\n", httpCode, response.c_str());

        if (httpCode == 201 || httpCode == 200) {
          Serial.println("-> [ÉXITO] Lectura ambiental registrada correctamente.");
        } else if (httpCode == 401) {
          Serial.println("-> [ERROR 401] Token Bearer no válido.");
        } else if (httpCode == 422) {
          Serial.println("-> [ERROR 422] Parámetros rechazados por la API.");
        } else if (httpCode == 429) {
          Serial.println("-> [ERROR 429] Límite de tasa excedido (esperar 1 minuto).");
        }
      } else {
        Serial.printf("-> [ERROR HTTP] Error de comunicación: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
      Serial.println("--------------------------------------------------\n");
  }
  client.stop();
  // ***
}

// =======================================================================
// SETUP
// =======================================================================
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n=== ESTACIÓN AMBIENTAL FISI-UNAP - ESP32 ===");

  analogReadResolution(12);

  Serial.print("Conectando a Wi-Fi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n¡Wi-Fi conectado con éxito!");
  Serial.print("IP asignada: ");
  Serial.println(WiFi.localIP());

  // Configuración del servidor de tiempo NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("Sincronizando hora NTP con servidor pool.ntp.org...");
}

// =======================================================================
// LOOP
// =======================================================================
void loop() {
  unsigned long tiempoActual = millis();

  if (tiempoActual - ultimoEnvio >= INTERVALO_ENVIO_MS || ultimoEnvio == 0) {
    ultimoEnvio = tiempoActual;
    enviarLecturaAPI();
  }
}
